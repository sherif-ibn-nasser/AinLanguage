#include "Compiler.hpp"
#include "AinException.hpp"
#include "Assembler.hpp"
#include "BaseScope.hpp"
#include "BoolValue.hpp"
#include "CharValue.hpp"
#include "DoubleValue.hpp"
#include "FloatValue.hpp"
#include "FunDecl.hpp"
#include "FunParam.hpp"
#include "FunScope.hpp"
#include "BuiltInFunScope.hpp"
#include "IExpression.hpp"
#include "IfStatement.hpp"
#include "IntValue.hpp"
#include "KeywordToken.hpp"
#include "LongValue.hpp"
#include "NonStaticFunInvokeExpression.hpp"
#include "NonStaticVarAccessExpression.hpp"
#include "OperatorFunInvokeExpression.hpp"
#include "OperatorFunctions.hpp"
#include "PackageScope.hpp"
#include "SetOperatorExpression.hpp"
#include "SharedPtrTypes.hpp"
#include "StmListScope.hpp"
#include "StringValue.hpp"
#include "ThisVarAccessExpression.hpp"
#include "Type.hpp"
#include "ULongValue.hpp"
#include "VarAccessExpression.hpp"
#include "Variable.hpp"
#include "WhileStatement.hpp"
#include "BuiltInFilePaths.hpp"
#include "string_helper.hpp"
#include <cassert>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

void Compiler::visit(PackageScope* scope){
    for(auto fileIterator:scope->getFiles()){
        fileIterator.second->accept(this);
    }
    for(auto packageIterator:scope->getPackages()){
        packageIterator.second->accept(this);
    }
}

void Compiler::visit(FileScope* scope){
    scope->getGlobalVarsInitStmList()->accept(this);
}

void Compiler::visit(ClassScope* scope){
    scope->getVarsInitStmList()->accept(this);
}

void Compiler::visit(FunScope* scope){

    auto decl=scope->getDecl();
    auto isConstructor=decl->isConstructor();
    auto parentClass=BaseScope::getContainingClass(scope->getParentScope()); // may not exist if it's a global function
    auto isMain=scope->getName()==L"البداية";
    std::wstring label,comment;

    if (isConstructor){
        label=L"constructor"+std::to_wstring(++constructorLabelsSize);
        comment=L"دالة "+decl->returnType->getClassScope()->getName()+L"::"+decl->toString();
    }

    else if (parentClass){
        label=L"method"+std::to_wstring(++methodLabelsSize);
        comment=L"دالة "+parentClass->getName()+L"::"+decl->toString();
    }

    else if (isMain){
        label=L"_start";
        comment=L"دالة البداية";
    }

    else{
        label=L"fun"+std::to_wstring(++funLabelsSize);
        comment=L"دالة "+scope->getDecl()->toString();
    }
    
    labelsAsm[scope]=Assembler::AsmLabel{.label=label, .comment=comment};

    auto prevLabelAsm=currentAsmLabel;
    auto prevLoopsLabelsSize=currentLoopLabelsSize;
    auto prevIfLabelsSize=currentIfLabelsSize;
    auto prevLogicalShortcutsLabelsSize=currentLogicalShortcutsLabelsSize;

    currentAsmLabel=&labelsAsm[scope];
    currentLoopLabelsSize=0;
    currentIfLabelsSize=0;
    currentLogicalShortcutsLabelsSize=0;

    *currentAsmLabel+=Assembler::push(Assembler::RBP());
    *currentAsmLabel+=Assembler::mov(Assembler::RBP(), Assembler::RSP());

    if(isConstructor)
        decl->returnType->getClassScope()->accept(this);

    // locals contains params, so we don't offset params
    auto params=scope->getParamsFromLocals();

    auto allLocalsSize=getLocalsSize(scope)-getVariablesSize(params);

    *currentAsmLabel+=Assembler::reserveSpaceOnStack(allLocalsSize);

    for(auto stm:*scope->getStmList()){
        stm->accept(this);
    }

    /* TODO: The following code should be removed after doing data-flow analysis*/

    if(isConstructor)
        *currentAsmLabel+=Assembler::mov(Assembler::RAX(),Assembler::RBX());

    else if(*scope->getReturnType()==*Type::UNIT)
        *currentAsmLabel+=Assembler::zero(Assembler::RAX());

    *currentAsmLabel+=Assembler::mov(Assembler::RSP(), Assembler::RBP());
    *currentAsmLabel+=Assembler::pop(Assembler::RBP());
    
    if(isMain){
        *currentAsmLabel+=Assembler::exit(0, L"إنهاء البرنامج");
        startAsmLabel=currentAsmLabel;
    }
    else
        *currentAsmLabel+=Assembler::ret();

    currentAsmLabel=prevLabelAsm;
    currentLoopLabelsSize=prevLoopsLabelsSize;
    currentIfLabelsSize=prevIfLabelsSize;
    currentLogicalShortcutsLabelsSize=prevLogicalShortcutsLabelsSize;
    
}

void Compiler::visit(BuiltInFunScope* scope){
    *currentAsmLabel+=scope->getGeneratedAsm(this);
}

void Compiler::visit(LoopScope* scope){
    for(auto stm:*scope->getStmList()){
        stm->accept(this);
    }
}

void Compiler::visit(StmListScope* scope){
    for(auto stm:*scope->getStmList()){
        stm->accept(this);
    }
}

void Compiler::visit(VarStm* stm){
    auto var=stm->getVar().get();

    auto isGlobal=inUseGlobalVariables.find(var)!=inUseGlobalVariables.end();

    if (currentAsmLabel==initAsmLabel&&!isGlobal)
        return;

    auto varSize=getVariableSize(var);
    auto offset=offsets[var];
    auto comment=((*var->isValue())?L"تعريف ثابت ":L"تعريف متغير ")+*var->getName()+L": "+var->getType()->getClassScope()->getName();
    stm->getEx()->accept(this);
    *currentAsmLabel+=Assembler::mov(
        Assembler::addressMov(offset.reg, offset.value),
        Assembler::RAX(varSize),
        Assembler::AsmInstruction::IMPLICIT,
        comment
    );
}

void Compiler::visit(AssignStatement* stm){
    leftAssign(stm->getLeft().get());
    stm->getRight()->accept(this);
    rightAssign(stm->getLeft().get());
}

void Compiler::visit(AugmentedAssignStatement* stm){
    if (stm->isOpFunExplicit()) {

        NonStaticFunInvokeExpression ex(
            stm->getLineNumber(),
            stm->getOpFun()->getName(),
            std::make_shared<std::vector<SharedIExpression>>(std::vector{stm->getRight()}),
            stm->getLeft()
        );
        ex.setFun(stm->getOpFun());
        ex.setReturnType(stm->getOpFun()->getReturnType());

        if(stm->getLeft()->getReturnType()==Type::STRING)
            invokeInsideString(&ex);
        else
            invokeNonStaticFun(&ex);

        return;
    }
    if(auto isBuiltIn=std::dynamic_pointer_cast<BuiltInFunScope>(stm->getOpFun())){
        leftAssign(stm->getLeft().get());
        *currentAsmLabel+=Assembler::push(Assembler::RAX());
        stm->getRight()->accept(this);
        stm->getOpFun()->accept(this);
        rightAssign(stm->getLeft().get());
    }else{
        auto rightSize=Type::getSize(stm->getRight()->getReturnType().get());
        
        *currentAsmLabel+=Assembler::push(Assembler::RBX());
        leftAssign(stm->getLeft().get());
        
        *currentAsmLabel+=Assembler::mov(Assembler::RBX(), Assembler::RAX());
        
        *currentAsmLabel+=Assembler::reserveSpaceOnStack(rightSize);
        stm->getRight()->accept(this);
        *currentAsmLabel+=
            Assembler::mov(
                Assembler::addressMov(Assembler::RSP()),
                Assembler::RAX(rightSize)
            );
        auto fun=stm->getOpFun().get();
        if (labelsAsm.find(fun)==labelsAsm.end())
            fun->accept(this);
        *currentAsmLabel+=Assembler::call(
            Assembler::label(labelsAsm[fun].label),
            L"استدعاء دالة "+fun->getParentScope()->getName()+L"::"+fun->getDecl()->toString()
        );
        Assembler::removeReservedSpaceFromStack(rightSize);
        
        rightAssign(stm->getLeft().get());

        *currentAsmLabel+=Assembler::pop(Assembler::RBX());
    }
}

void Compiler::visit(IfStatement* stm){

    auto elseScope=stm->getElseScope().get();

    auto ifNumStr=std::to_wstring(++currentIfLabelsSize);
    auto ifLabelStr=L"if"+ifNumStr;
    auto elseLabelStr=L"else"+ifNumStr;
    auto endLabelStr=L"end"+ifNumStr;
    
    auto ifLabel=Assembler::localLabel(ifLabelStr);
    auto elseLabel=Assembler::localLabel(elseLabelStr);
    auto endLabel=Assembler::localLabel(endLabelStr);

    *currentAsmLabel+=ifLabel;

    auto conditionalJumpLabelStr=(elseScope)?elseLabelStr:endLabelStr;
    
    optimizeNegatedConditionalJumpInstruction(
        stm->getIfCondition().get(),
        Assembler::label(L"."+conditionalJumpLabelStr)
    );

    stm->getIfScope()->accept(this);

    if(elseScope){
        *currentAsmLabel+=Assembler::jmp(Assembler::label(L"."+endLabelStr));
        *currentAsmLabel+=elseLabel;
        elseScope->accept(this);
    }

    *currentAsmLabel+=endLabel;
}

void Compiler::visit(WhileStatement* stm){
    visitLoopStm(stm);
}

void Compiler::visit(DoWhileStatement* stm){
    visitLoopStm(stm,true);
}

void Compiler::visit(BreakStatement* stm){
    auto loopNumStr=std::to_wstring(currentLoopLabelsSize);
    *currentAsmLabel+=
        Assembler::jmp(
            Assembler::label(L".break"+loopNumStr),
            KeywordToken::BREAK.getVal() // instruction comment
    );
}

void Compiler::visit(ContinueStatement* stm){
    auto loopNumStr=std::to_wstring(currentLoopLabelsSize);
    *currentAsmLabel+=
        Assembler::jmp(
            Assembler::label(L".continue"+loopNumStr),
            KeywordToken::CONTINUE.getVal() // instruction comment
    );
}

void Compiler::visit(ReturnStatement* stm){
    auto fun=BaseScope::getContainingFun(stm->getRunScope()).get();

    stm->getEx()->accept(this);

    if(labelsAsm[fun].label==L"_start"){
        *currentAsmLabel+=Assembler::exit(0, L"إنهاء البرنامج");
        return;
    }

    if(fun->getDecl()->isConstructor())
        *currentAsmLabel+=
            Assembler::mov(
                Assembler::RAX(),
                Assembler::addressMov(Assembler::RBX())
            );

    *currentAsmLabel+=Assembler::mov(Assembler::RSP(), Assembler::RBP());
    *currentAsmLabel+=Assembler::pop(Assembler::RBP());
    *currentAsmLabel+=Assembler::ret();
}

void Compiler::visit(ExpressionStatement* stm){
    stm->getEx()->accept(this);
}

void Compiler::visit(VarAccessExpression* ex){
    auto var=ex->getVar().get();
    auto varSize=getVariableSize(var);
    auto offset=offsets[var];
    auto comment=((*var->isValue())?L"الوصول لثابت ":L"الوصول لمتغير ")+*var->getName();

    *currentAsmLabel+=
        Assembler::mov(
            Assembler::RAX(varSize),
            Assembler::addressMov(offset.reg, offset.value),
            Assembler::AsmInstruction::IMPLICIT,
            comment
        );

    if (offset.reg.value.find(L"var")!=0)
        return;

    // The variable is global
    
    if(inUseGlobalVariables.find(var)==inUseGlobalVariables.end()){
        inUseGlobalVariables[var]={};
        bssAsm+=L"\t"+offset.reg.value+L":\t";
        switch (varSize) {
            case Assembler::AsmInstruction::BYTE:
                bssAsm+=L"RESB 1\n";break;
            case Assembler::AsmInstruction::WORD:
                bssAsm+=L"RESW 1\n";break;
            case Assembler::AsmInstruction::DWORD:
                bssAsm+=L"RESD 1\n";break;
            case Assembler::AsmInstruction::QWORD:
                bssAsm+=L"RESQ 1\n";break;
        }
    }
}

void Compiler::visit(FunInvokeExpression* ex){

    callFunAsm(
        ex->getFun().get(),
        ex->getArgs()
    );

}

void Compiler::visit(NewObjectExpression* ex){
    
    addAinAllocAsm();

    auto size=ex->getReturnType()->getClassScope()->getSize();
    *currentAsmLabel+=Assembler::push(Assembler::imm(std::to_wstring(size))); // The size arg
    *currentAsmLabel+=Assembler::call(Assembler::label(labelsAsm[AIN_ALLOC].label)); // call ainalloc
    *currentAsmLabel+=Assembler::pop(Assembler::RDX()); // The size arg

    callFunAsm(
        ex->getConstructor().get(),
        ex->getArgs(),
        true
    );
}

void Compiler::visit(NewArrayExpression* ex){
    addAinAllocAsm();

    auto capExs=ex->getCapacities();
    auto capCount=capExs.size();
    auto elementSize=Type::getSize(ex->getReturnType()->asArray()->getType().get());

    if(capCount==1){
        capExs[0]->accept(this);

        *currentAsmLabel+=Assembler::push(Assembler::RAX()); // push the evaluated user size

        *currentAsmLabel+=Assembler::lea(
            Assembler::RAX(),
            Assembler::addressLea(
                Assembler::RAX().value+L"*"+std::to_wstring(elementSize)+L"+8" // add 8 bytes for the user size of the array
            )
        );

        *currentAsmLabel+=Assembler::push(Assembler::RAX()); // push the size arg for ainalloc

        *currentAsmLabel+=Assembler::call(Assembler::label(labelsAsm[AIN_ALLOC].label), L"استدعاء دالة احجز(كبير)");

        *currentAsmLabel+=Assembler::pop(Assembler::RDX()); // the size arg for ainalloc
        *currentAsmLabel+=Assembler::pop(Assembler::RDX()); // the user size

        *currentAsmLabel+=Assembler::mov(
            Assembler::addressMov(Assembler::RAX()),
            Assembler::RDX()
        ); // write the evaluated user size

        return;
    }

    addAinAllocateArrayAsm();
    
    *currentAsmLabel+=Assembler::reserveSpaceOnStack(capExs.size()*8);

    auto i=0;
    for(auto capEx:capExs){
        capEx->accept(this);
        *currentAsmLabel+=Assembler::mov(Assembler::addressMov(Assembler::RSP(), i), Assembler::RAX());
        i+=8;
    }

    *currentAsmLabel+=Assembler::push(Assembler::imm(std::to_wstring(capExs.size())));
    *currentAsmLabel+=Assembler::push(Assembler::RSP(), L"مُعامل الأبعاد: [كبير]"); // The array of dimensions args
    *currentAsmLabel+=Assembler::push(Assembler::imm(std::to_wstring(elementSize)), L"مُعامل حجم_العنصر: كبير");

    *currentAsmLabel+=Assembler::call(Assembler::label(labelsAsm[AIN_ALLOCATE_ARRAY].label), L"استدعاء دالة احجز_مصفوفة([كيبر]، كبير)"); // call ainallocatearray

    *currentAsmLabel+=Assembler::removeReservedSpaceFromStack(capExs.size()*8+3*8); // remove also the args of ainalocatearray, i.e. the 3 pushed values

}

void Compiler::visit(LiteralExpression* ex){

    auto value=ex->getValue();
    Assembler::AsmOperand imm;

    if(auto boolVal=std::dynamic_pointer_cast<BoolValue>(value)){
        if(!boolVal->getValue()){
            *currentAsmLabel+=Assembler::zero(Assembler::RAX());
            return;
        }
        imm=Assembler::imm(L"1");
    }

    else if(auto charVal=std::dynamic_pointer_cast<CharValue>(value)){
        auto val=charVal->toString();
        if(val==L"\n")
            imm=Assembler::imm(L"0x0a");
        else
            imm=Assembler::imm(L"\'"+val+L"\'");
    }

    else if(auto floatVal=std::dynamic_pointer_cast<FloatValue>(value)){
        union float_bytes {
            float fVal;
            int iVal;
        } data;
        data.fVal=floatVal->getValue();
        imm=Assembler::imm(std::to_wstring(data.iVal));
    }

    else if(auto doubleVal=std::dynamic_pointer_cast<DoubleValue>(value)){
        union double_bytes {
            double dVal;
            long long lVal;
        } data;
        data.dVal=doubleVal->getValue();
        imm=Assembler::imm(std::to_wstring(data.lVal));
    }

    else if(auto strVal=std::dynamic_pointer_cast<StringValue>(value)){
        auto wstr=strVal->toString();
        auto str=std::string(toCharPointer(wstr));

        addAinAllocAsm();
        
        *currentAsmLabel+=Assembler::push(
            Assembler::imm(std::to_wstring(str.size()+8)), // add 8 bytes for the size property
            L"مُعامل الحجم_بالبايت: كبير"
        );
        *currentAsmLabel+=Assembler::call(Assembler::label(labelsAsm[AIN_ALLOC].label), L"استدعاء دالة احجز(كبير)");
        *currentAsmLabel+=Assembler::pop(Assembler::RDX()); // size arg

        *currentAsmLabel+=Assembler::mov(
            Assembler::addressMov(Assembler::RAX()),
            Assembler::imm(std::to_wstring(str.size())),
            Assembler::AsmInstruction::QWORD
        );
        for (auto i = 0; i < str.size(); i += 4) {
            union double_bytes {
                char cVal[4]={0};
                int lVal;
            } data;

            auto substr=str.substr(i, 4);
            auto j=0;
            for(auto &c:substr)
                data.cVal[j++]=c;
            
            auto subStrImm=Assembler::imm(std::to_wstring(data.lVal));
            *currentAsmLabel+=Assembler::mov(
                Assembler::addressMov(Assembler::RAX(), i+8),
                subStrImm,
                Assembler::AsmInstruction::DWORD
            );
        }

        return;

    }

    else
        imm=Assembler::imm(value->toString());
    
    *currentAsmLabel+=Assembler::mov(Assembler::RAX(),imm);
}

void Compiler::visit(UnitExpression* ex){
    *currentAsmLabel+=Assembler::zero(Assembler::RAX());
}

void Compiler::visit(LogicalExpression* ex){

    auto shortcutLabelStr=L"logical_shortcut"+std::to_wstring(++currentLogicalShortcutsLabelsSize);
    auto shortcutLocalLabel=Assembler::localLabel(shortcutLabelStr);
    auto shortcutLabel=Assembler::label(L"."+shortcutLabelStr);

    ex->getLeft()->accept(this);
    
    auto AL=Assembler::RAX(Assembler::AsmInstruction::BYTE);
    *currentAsmLabel+=Assembler::test(AL, AL);
    
    switch (ex->getLogicalOp()) {
        case LogicalExpression::Operation::OR:{
            *currentAsmLabel+=Assembler::jnz(shortcutLabel,L"أو الشرطية");
            break;
        }
        case LogicalExpression::Operation::AND:{
            *currentAsmLabel+=Assembler::jz(shortcutLabel,L"و الشرطية");
            break;
        }
    }

    ex->getRight()->accept(this);
    
    *currentAsmLabel+=shortcutLocalLabel;

}

void Compiler::visit(NonStaticVarAccessExpression* ex){
    auto var=ex->getVar().get();
    auto varSize=getVariableSize(var);
    auto offset=offsets[var];
    auto comment=
        ((*var->isValue())?L"الوصول لثابت ":L"الوصول لمتغير ")
        +ex->getInside()->getReturnType()->getClassScope()->getName()
        +L"::"
        +*var->getName()
    ;

    ex->getInside()->accept(this);

    *currentAsmLabel+=
        Assembler::mov(
            Assembler::RAX(varSize),
            Assembler::addressMov(Assembler::RAX(), offset.value),
            Assembler::AsmInstruction::IMPLICIT,
            comment
        )
    ;
}

void Compiler::visit(NonStaticFunInvokeExpression* ex){

    if(ex->getInside()->getReturnType()==Type::STRING){
        invokeInsideString(ex);
        return;
    }

    if(auto builtIn=std::dynamic_pointer_cast<BuiltInFunScope>(ex->getFun())){
        if (*builtIn->getDecl()->isOperator)
            invokeNonStaticBuiltInFun(ex);
        else{
            ex->getInside()->accept(this);
            ex->getFun()->accept(this); // It will be inlined
        }
        return;
    }
    invokeNonStaticFun(ex);
}

void Compiler::visit(OperatorFunInvokeExpression* ex){

    if(ex->getInside()->getReturnType()==Type::STRING){
        invokeInsideString(ex);
        return;
    }

    if(auto builtIn=std::dynamic_pointer_cast<BuiltInFunScope>(ex->getFun())){
        invokeBuiltInOpFun(ex);
        return;
    }

    auto op=ex->getOp();

    if(
        op==OperatorFunInvokeExpression::Operator::PRE_INC
        ||
        op==OperatorFunInvokeExpression::Operator::PRE_DEC
        ||
        op==OperatorFunInvokeExpression::Operator::POST_INC
        ||
        op==OperatorFunInvokeExpression::Operator::POST_DEC
    ){

        *currentAsmLabel+=Assembler::push(Assembler::RBX());
        leftAssign(ex->getInside().get());

        switch(op){
            case OperatorFunInvokeExpression::Operator::POST_INC:
            case OperatorFunInvokeExpression::Operator::POST_DEC:
                *currentAsmLabel+=Assembler::push(Assembler::RAX()); // old val
            default:
                break;
        }

        *currentAsmLabel+=Assembler::mov(Assembler::RBX(), Assembler::RAX());

        auto fun=ex->getFun().get();
        if (labelsAsm.find(fun)==labelsAsm.end())
            fun->accept(this);
        *currentAsmLabel+=Assembler::call(
            Assembler::label(labelsAsm[fun].label),
            L"استدعاء دالة "+fun->getParentScope()->getName()+L"::"+fun->getDecl()->toString()
        );

        rightAssign(ex->getInside().get());

        switch(op){
            case OperatorFunInvokeExpression::Operator::POST_INC:
            case OperatorFunInvokeExpression::Operator::POST_DEC:
                *currentAsmLabel+=Assembler::pop(Assembler::RAX()); // old val
            default:
                break;
        }

        *currentAsmLabel+=Assembler::pop(Assembler::RBX());

        return;
    }

    invokeNonStaticFun(ex);

    switch (op) {
        case OperatorFunInvokeExpression::Operator::NOT_EQUAL:
        case OperatorFunInvokeExpression::Operator::LESS:
        case OperatorFunInvokeExpression::Operator::LESS_EQUAL:
        case OperatorFunInvokeExpression::Operator::GREATER:
        case OperatorFunInvokeExpression::Operator::GREATER_EQUAL:
            break;
        default:
            return;
    }

    *currentAsmLabel+=Assembler::cmp(Assembler::RAX(), Assembler::imm(L"0"));

    auto AL=Assembler::RAX(Assembler::AsmInstruction::BYTE);

    switch (op) {
        case OperatorFunInvokeExpression::Operator::NOT_EQUAL:
            *currentAsmLabel+=Assembler::setz(AL);
            break;
        case OperatorFunInvokeExpression::Operator::LESS:
            *currentAsmLabel+=Assembler::setl(AL);
            break;
        case OperatorFunInvokeExpression::Operator::LESS_EQUAL:
            *currentAsmLabel+=Assembler::setle(AL);
            break;
        case OperatorFunInvokeExpression::Operator::GREATER:
            *currentAsmLabel+=Assembler::setg(AL);
            break;
        case OperatorFunInvokeExpression::Operator::GREATER_EQUAL:
            *currentAsmLabel+=Assembler::setge(AL);
            break;
        default:{}
    }
}

void Compiler::visit(SetOperatorExpression* ex){
    
    if(ex->isOpFunExplicit()){
        auto funOfGet=ex->getExOfGet()->getFun();
        auto funOfOp=ex->getFunOfOp();

        auto getEx=std::make_shared<NonStaticFunInvokeExpression>(
            ex->getLineNumber(),
            funOfGet->getName(),
            std::make_shared<std::vector<SharedIExpression>>(std::vector{ex->getIndexEx()}),
            ex->getExHasGetOp()
        );
        getEx->setFun(funOfGet);
        getEx->setReturnType(funOfGet->getReturnType());

        NonStaticFunInvokeExpression explicitAssignEx(
            ex->getLineNumber(),
            ex->getFunOfOp()->getName(),
            std::make_shared<std::vector<SharedIExpression>>(std::vector{ex->getValueEx()}),
            getEx
        );
        explicitAssignEx.setFun(funOfOp);
        explicitAssignEx.setReturnType(funOfOp->getReturnType());

        invokeNonStaticFun(&explicitAssignEx);

        return;
    }

    auto op=ex->getOp();

    auto funOfGet=ex->getExOfGet()->getFun().get();
    auto funOfOp=ex->getFunOfOp().get();
    auto funOfSet=ex->getFunOfSet().get();
    auto arrayElementSize=Type::getSize(funOfGet->getReturnType().get());

    auto isBuiltInSetOp=dynamic_cast<BuiltInFunScope*>(funOfSet);
    auto isBuiltInAugOp=dynamic_cast<BuiltInFunScope*>(funOfOp);

    if(isBuiltInSetOp){
        
        ex->getExHasGetOp()->accept(this);
        *currentAsmLabel+=Assembler::push(Assembler::RAX());
        ex->getIndexEx()->accept(this);
        addArrayGetOpAsm(arrayElementSize);
        // RDI contains the array pointer
        // RSI contains the address of element (the index is performed relative to the array pointer)
        // RAX contains the value returned by get op

        if(isBuiltInAugOp){
            compileBuiltInSetWithBuiltInAugOp(op, arrayElementSize, ex->getValueEx(), funOfOp);
        }
        else{
            compileBuiltInSetWithNonBuiltInAugOp(op, arrayElementSize, ex->getValueEx(), funOfOp);
        }

    }
    else{
        /** Stack evaluation
         * Old object
         * Old object, Ex has get
         * Old object, Ex has get, Index
         * Old object, Ex has get, Index, (Value returned by get: for binary operators)
         * Old object, Ex has get, Index, Value to set
        */
        auto opArgSize=(ex->getValueEx())?Type::getSize(ex->getValueEx()->getReturnType().get()):0;
        auto valueToSetSize=Type::getSize(funOfSet->getDecl()->params->at(1)->type.get());
        auto indexTypeSize=Type::getSize(ex->getIndexEx()->getReturnType().get());

        *currentAsmLabel+=Assembler::push(Assembler::RBX()); // prev object; push it first to not add additional mov instruction

        ex->getExHasGetOp()->accept(this);
        *currentAsmLabel+=Assembler::push(Assembler::RAX()); // ex has get

        ex->getIndexEx()->accept(this);
        *currentAsmLabel+=Assembler::mov(Assembler::RBX(), Assembler::addressMov(Assembler::RSP())); // ex has get

        if(indexTypeSize==8)
            *currentAsmLabel+=Assembler::push(Assembler::RAX()); // push index as arg of get method
        else{
            *currentAsmLabel+=Assembler::reserveSpaceOnStack(indexTypeSize);
            *currentAsmLabel+=Assembler::mov(Assembler::addressMov(Assembler::RSP()), Assembler::RAX(indexTypeSize));
        }

        if(labelsAsm.find(funOfGet)==labelsAsm.end())
            funOfGet->accept(this);

        *currentAsmLabel+=Assembler::call(
            Assembler::label(labelsAsm[funOfGet].label),
            L"استدعاء دالة "+*ex->getExHasGetOp()->getReturnType()->getName()+L"::"+funOfGet->getDecl()->toString()
        );

        // RAX contains the value returned by get
        // Then it will contain the value to set
        if(isBuiltInAugOp){
            switch (op) {
                case SetOperatorExpression::Operator::PRE_INC:
                case SetOperatorExpression::Operator::POST_INC:{
                    *currentAsmLabel+=Assembler::inc(Assembler::RAX());
                    break;
                }
                case SetOperatorExpression::Operator::PRE_DEC:
                case SetOperatorExpression::Operator::POST_DEC:{
                    *currentAsmLabel+=Assembler::dec(Assembler::RAX());
                    break;
                }
                default:{
                    // restore old RBX
                    *currentAsmLabel+=Assembler::mov(Assembler::RBX(), Assembler::addressMov(Assembler::RSP(), indexTypeSize+8));
                    *currentAsmLabel+=Assembler::push(Assembler::RAX()); // The value returned by get
                    ex->getValueEx()->accept(this);
                    funOfOp->accept(this);
                }
            }
        }

        else{

            // opArgSize indicates the op is binary or unary

            if(opArgSize!=0){
                // restore old RBX
                *currentAsmLabel+=Assembler::mov(Assembler::RBX(), Assembler::addressMov(Assembler::RSP(), indexTypeSize+8));
                *currentAsmLabel+=Assembler::push(Assembler::RAX()); // The value returned by get
                ex->getValueEx()->accept(this);
                *currentAsmLabel+=Assembler::pop(Assembler::RBX()); // The value returned by get, i.e. the value to call op fun on it
            }
            else
                *currentAsmLabel+=Assembler::mov(Assembler::RBX(), Assembler::RAX()); // The value returned by get, i.e. the value to call op fun on it
            
            if(opArgSize==8)
                *currentAsmLabel+=Assembler::push(Assembler::RAX()); // The arg of op fun
            else if(opArgSize!=0){
                *currentAsmLabel+=Assembler::reserveSpaceOnStack(opArgSize); // The value to call op fun on it, i.e. the arg of op fun
                *currentAsmLabel+=Assembler::mov(Assembler::addressMov(Assembler::RSP()), Assembler::RAX(opArgSize));
            }

            if(labelsAsm.find(funOfOp)==labelsAsm.end())
                funOfOp->accept(this);

            *currentAsmLabel+=Assembler::call(
                Assembler::label(labelsAsm[funOfOp].label),
                L"استدعاء دالة "+funOfOp->getParentScope()->getName()+L"::"+funOfOp->getDecl()->toString()
            );
            
            if(opArgSize==8)
                *currentAsmLabel+=Assembler::pop(Assembler::RDX()); // The value to call op fun on it, i.e. the arg of op fun
            else if(opArgSize!=0)
                *currentAsmLabel+=Assembler::removeReservedSpaceFromStack(opArgSize); // The value to call op fun on it, i.e. the arg of op fun

        }

        // ex has get
        *currentAsmLabel+=Assembler::mov(Assembler::RBX(), Assembler::addressMov(Assembler::RSP(), indexTypeSize));

        if(valueToSetSize==8)
            *currentAsmLabel+=Assembler::push(Assembler::RAX()); // push value to set
        else{
            *currentAsmLabel+=Assembler::reserveSpaceOnStack(valueToSetSize);
            *currentAsmLabel+=Assembler::mov(Assembler::addressMov(Assembler::RSP()), Assembler::RAX(valueToSetSize));
        }

        if(labelsAsm.find(funOfSet)==labelsAsm.end())
            funOfSet->accept(this);

        *currentAsmLabel+=Assembler::call(
            Assembler::label(labelsAsm[funOfSet].label),
            L"استدعاء دالة "+*ex->getExHasGetOp()->getReturnType()->getName()+L"::"+funOfSet->getDecl()->toString()
        );

        if(op==SetOperatorExpression::Operator::PRE_INC||op==SetOperatorExpression::Operator::PRE_DEC){
            if(valueToSetSize==8)
                *currentAsmLabel+=Assembler::pop(Assembler::RDX()); // pop value to set
            else
                *currentAsmLabel+=Assembler::removeReservedSpaceFromStack(valueToSetSize); // remove value to set from the stack

            *currentAsmLabel+=Assembler::call(
                Assembler::label(labelsAsm[funOfGet].label),
                L"استدعاء دالة "+*ex->getExHasGetOp()->getReturnType()->getName()+L"::"+funOfGet->getDecl()->toString()
            );

            *currentAsmLabel+=Assembler::removeReservedSpaceFromStack(indexTypeSize+8); // remove 8 bytes of ex has get and index from the stack
        }

        else{

            if(isBuiltInAugOp&&(op==SetOperatorExpression::Operator::POST_INC||op==SetOperatorExpression::Operator::POST_DEC)){

                *currentAsmLabel+=Assembler::mov(Assembler::RAX(), Assembler::addressMov(Assembler::RSP()));

                if(op==SetOperatorExpression::Operator::POST_INC)
                    *currentAsmLabel+=Assembler::dec(Assembler::RAX());
                else
                    *currentAsmLabel+=Assembler::inc(Assembler::RAX());
            }


            *currentAsmLabel+=Assembler::removeReservedSpaceFromStack(indexTypeSize+valueToSetSize+8); // remove 8 bytes of ex has get, index and value to set from the stack
        }

        *currentAsmLabel+=Assembler::pop(Assembler::RBX()); // old RBX

    }
   
}

void Compiler::visit(ThisExpression* ex){
    
    *currentAsmLabel+=
        Assembler::mov(
            Assembler::RAX(),
            Assembler::RBX(),
            Assembler::AsmInstruction::IMPLICIT,
            L"هذا"
        )
    ;
}

void Compiler::visit(ThisVarAccessExpression* ex){
    auto var=ex->getVar().get();
    auto varSize=getVariableSize(var);
    auto offset=offsets[var];
    auto comment=
        ((*var->isValue())?L"الوصول لثابت ":L"الوصول لمتغير ")
        +ex->getClassScope()->getName()
        +L"::"
        +*var->getName()
    ;

    *currentAsmLabel+=
        Assembler::mov(
            Assembler::RAX(varSize),
            Assembler::addressMov(offset.reg, offset.value),
            Assembler::AsmInstruction::IMPLICIT,
            comment
        )
    ;
}

void Compiler::visit(ThisFunInvokeExpression* ex){
    callFunAsm(ex->getFun().get(),ex->getArgs());
}

std::wstring Compiler::getAssemblyFile(){

    if(!bssAsm.empty()){
        bssAsm=L"section .bss\n"+bssAsm+L"\n";
        initAsmLabel=&labelsAsm[NULL];
        currentAsmLabel=initAsmLabel;
        currentAsmLabel->label=L"init";
        currentAsmLabel->comment=L"تهيئة";
        PackageScope::AIN_PACKAGE->accept(this);
    }

    auto asmFile=dataAsm+L"\n"+bssAsm+textAsm;

    for(auto labelAsmIt:labelsAsm){

        if(initAsmLabel&&labelAsmIt.second.label==initAsmLabel->label)
            continue;

        if(initAsmLabel&&labelAsmIt.second.label==startAsmLabel->label){
            startAsmLabel->instructions.insert(
                startAsmLabel->instructions.begin(),
                initAsmLabel->instructions.begin(),
                initAsmLabel->instructions.end()
            );
            asmFile+=L"\n\n"+startAsmLabel->getAsmText();
            continue;
        }

        asmFile+=L"\n\n"+labelAsmIt.second.getAsmText();
    }
    return asmFile;
}

int Compiler::getVariableSize(Variable* var){
    return Type::getSize(var->getType().get());
}

int Compiler::getVariablesSize(SharedMap<std::wstring, SharedVariable> vars){
    auto size=0;
    for (auto &varIt:*vars){
        size+=getVariableSize(varIt.second.get());
    }
    return size;
}

void Compiler::optimizeConditionalJumpInstruction(IExpression* condition, Assembler::AsmOperand label, std::wstring comment){
    
    condition->accept(this);
  
    auto jumpType=Assembler::AsmInstruction::JZ;

    auto lastInstruction=&currentAsmLabel->instructions.back();

    switch (lastInstruction->type) {
        case Assembler::AsmInstruction::SETZ:
            jumpType=Assembler::AsmInstruction::JZ;break;
        case Assembler::AsmInstruction::SETNZ:
            jumpType=Assembler::AsmInstruction::JNZ;break;
        case Assembler::AsmInstruction::SETS:
            jumpType=Assembler::AsmInstruction::JS;break;
        case Assembler::AsmInstruction::SETNS:
            jumpType=Assembler::AsmInstruction::JNS;break;
        case Assembler::AsmInstruction::SETG:
            jumpType=Assembler::AsmInstruction::JG;break;
        case Assembler::AsmInstruction::SETGE:
            jumpType=Assembler::AsmInstruction::JGE;break;
        case Assembler::AsmInstruction::SETL:
            jumpType=Assembler::AsmInstruction::JL;break;
        case Assembler::AsmInstruction::SETLE:
            jumpType=Assembler::AsmInstruction::JLE;break;
        case Assembler::AsmInstruction::SETA:
            jumpType=Assembler::AsmInstruction::JA;break;
        case Assembler::AsmInstruction::SETAE:
            jumpType=Assembler::AsmInstruction::JAE;break;
        case Assembler::AsmInstruction::SETB:
            jumpType=Assembler::AsmInstruction::JB;break;
        case Assembler::AsmInstruction::SETBE:
            jumpType=Assembler::AsmInstruction::JBE;break;
        default:{
            auto AL=Assembler::RAX(Assembler::AsmInstruction::BYTE);
            *currentAsmLabel+=Assembler::test(AL, AL);
            *currentAsmLabel+=Assembler::jnz(label,comment);
            return;
        }
    }

    *lastInstruction=Assembler::AsmInstruction{
        .type=jumpType,
        .operands={label},
        .comment=comment
    };

}

void Compiler::optimizeNegatedConditionalJumpInstruction(IExpression* condition, Assembler::AsmOperand label, std::wstring comment){
    
    condition->accept(this);
    
    auto jumpType=Assembler::AsmInstruction::JZ;

    auto lastInstruction=&currentAsmLabel->instructions.back();

    switch (lastInstruction->type) {
        case Assembler::AsmInstruction::SETZ:
            jumpType=Assembler::AsmInstruction::JNZ;break;
        case Assembler::AsmInstruction::SETNZ:
            jumpType=Assembler::AsmInstruction::JZ;break;
        case Assembler::AsmInstruction::SETS:
            jumpType=Assembler::AsmInstruction::JNS;break;
        case Assembler::AsmInstruction::SETNS:
            jumpType=Assembler::AsmInstruction::JS;break;
        case Assembler::AsmInstruction::SETG:
            jumpType=Assembler::AsmInstruction::JLE;break;
        case Assembler::AsmInstruction::SETGE:
            jumpType=Assembler::AsmInstruction::JL;break;
        case Assembler::AsmInstruction::SETL:
            jumpType=Assembler::AsmInstruction::JGE;break;
        case Assembler::AsmInstruction::SETLE:
            jumpType=Assembler::AsmInstruction::JG;break;
        case Assembler::AsmInstruction::SETA:
            jumpType=Assembler::AsmInstruction::JBE;break;
        case Assembler::AsmInstruction::SETAE:
            jumpType=Assembler::AsmInstruction::JB;break;
        case Assembler::AsmInstruction::SETB:
            jumpType=Assembler::AsmInstruction::JAE;break;
        case Assembler::AsmInstruction::SETBE:
            jumpType=Assembler::AsmInstruction::JA;break;
        default:{
            auto AL=Assembler::RAX(Assembler::AsmInstruction::BYTE);
            *currentAsmLabel+=Assembler::test(AL, AL);
            *currentAsmLabel+=Assembler::jz(label,comment);
            return;
        }
    }

    *lastInstruction=Assembler::AsmInstruction{
        .type=jumpType,
        .operands={label},
        .comment=comment
    };
}

void Compiler::visitLoopStm(WhileStatement *stm, bool isDoWhileStm){
    
    auto loopNumStr=std::to_wstring(++currentLoopLabelsSize);
    auto loopLabelStr=L"loop"+loopNumStr;
    auto continueLabelStr=L"continue"+loopNumStr;
    auto breakLabelStr=L"break"+loopNumStr;
    
    auto loopLabel=Assembler::localLabel(loopLabelStr);
    auto continueLabel=Assembler::localLabel(continueLabelStr);
    auto breakLabel=Assembler::localLabel(breakLabelStr);

    if(!isDoWhileStm)
        *currentAsmLabel+=Assembler::jmp(Assembler::label(L"."+continueLabelStr));

    *currentAsmLabel+=loopLabel;

    stm->getLoopScope()->accept(this);

    *currentAsmLabel+=continueLabel;

    optimizeConditionalJumpInstruction(
        stm->getCondition().get(),
        Assembler::label(L"."+loopLabelStr)
    );

    *currentAsmLabel+=breakLabel;
}

int Compiler::getLocalsSize(StmListScope* scope){
    int size=getVariablesSize(scope->getLocals());
    for (auto stm : *scope->getStmList()) {
        if (auto ifStm=std::dynamic_pointer_cast<IfStatement>(stm)){
            size+=getLocalsSize(ifStm->getIfScope().get());
            if(auto elseScope=ifStm->getElseScope())
                size+=getLocalsSize(elseScope.get());
        }
        else if (auto loopStm=std::dynamic_pointer_cast<WhileStatement>(stm)){
            size+=getLocalsSize(loopStm->getLoopScope().get());
        }
    }
    return size;
}

void Compiler::invokeBuiltInOpFun(OperatorFunInvokeExpression* ex){
    auto op=ex->getOp();

    auto inside=ex->getInside();
    inside->accept(this);
    auto insideSize=Type::getSize(inside->getReturnType().get());
    auto argSize=0; // used for binary operators later

    auto isUnsigned=
        *inside->getReturnType()==*Type::UBYTE
        ||
        *inside->getReturnType()==*Type::UINT
        ||
        *inside->getReturnType()==*Type::ULONG
    ;

    switch (op) {
        case OperatorFunInvokeExpression::Operator::PLUS:
        case OperatorFunInvokeExpression::Operator::MINUS:
        case OperatorFunInvokeExpression::Operator::TIMES:
        case OperatorFunInvokeExpression::Operator::DIV:
        case OperatorFunInvokeExpression::Operator::MOD:
        case OperatorFunInvokeExpression::Operator::XOR:
        case OperatorFunInvokeExpression::Operator::BIT_OR:
        case OperatorFunInvokeExpression::Operator::BIT_AND:
        case OperatorFunInvokeExpression::Operator::SHR:
        case OperatorFunInvokeExpression::Operator::SHL:
        case OperatorFunInvokeExpression::Operator::EQUAL_EQUAL:
        case OperatorFunInvokeExpression::Operator::NOT_EQUAL:
        case OperatorFunInvokeExpression::Operator::LESS:
        case OperatorFunInvokeExpression::Operator::LESS_EQUAL:
        case OperatorFunInvokeExpression::Operator::GREATER:
        case OperatorFunInvokeExpression::Operator::GREATER_EQUAL:{
            argSize=Type::getSize((*ex->getArgs())[0]->getReturnType().get());
            addInstructionToConvertBetweenDataTypes(insideSize, argSize, isUnsigned);
        }
        default:{}
    }

    auto returnType=ex->getReturnType().get();
    auto returnTypeSize=Type::getSize(returnType);
    auto sizeAsm=Assembler::size(returnTypeSize);

    auto isPreInc=op==OperatorFunInvokeExpression::Operator::PRE_INC;
    auto isPreDec=op==OperatorFunInvokeExpression::Operator::PRE_DEC;
    auto isPostInc=op==OperatorFunInvokeExpression::Operator::POST_INC;
    auto isPostDec=op==OperatorFunInvokeExpression::Operator::POST_DEC;

    if(isPreInc||isPostInc||isPreDec||isPostDec){

        auto instructions=&currentAsmLabel->instructions;

        if(!IExpression::isAssignableExpression(inside)){
            // The inc/dec called as a function on non-assignable ex
            if (isPreInc||isPostInc)
                *currentAsmLabel+=Assembler::inc(Assembler::RAX());
            else
                *currentAsmLabel+=Assembler::dec(Assembler::RAX());

            return;
        }

        auto isNonStaticVarAccessEx=std::dynamic_pointer_cast<NonStaticVarAccessExpression>(inside);
        
        auto lastInstruction=instructions->back();

        auto lastInstructionRAXSize=(isNonStaticVarAccessEx)?Assembler::getSizeOfReg(lastInstruction.operands[0]):0;

        auto comment=lastInstruction.comment;
        std::wstring oldComment=L"الوصول ل";
        auto newComment=(isPreInc||isPostInc)?L"زيادة ":L"نقصان ";
        comment.replace(0, oldComment.size(), newComment);

        if (isPreInc||isPreDec)
            instructions->pop_back();
        else if(isNonStaticVarAccessEx)
            (&instructions->back())->operands[0]=Assembler::RDX(lastInstructionRAXSize);
        
        
        if (isPreInc||isPostInc)
            *currentAsmLabel+=Assembler::inc(
                lastInstruction.operands[1],
                sizeAsm,
                comment
            );
        else
            *currentAsmLabel+=Assembler::dec(
                lastInstruction.operands[1],
                sizeAsm,
                comment
            );

        
        if (isPreInc||isPreDec)
            *currentAsmLabel+=lastInstruction;
        else if(isNonStaticVarAccessEx)
            *currentAsmLabel+=Assembler::mov(Assembler::RAX(), Assembler::RDX());

        return;
    }

    for(auto arg:*ex->getArgs()){
        *currentAsmLabel+=Assembler::push(Assembler::RAX()); // push (inside expression or the index arg for set operator) to the stack
        arg->accept(this);
    }

    if(op==OperatorFunInvokeExpression::Operator::GET){
        addArrayGetOpAsm(returnTypeSize);
        return;
    }

    else if(op==OperatorFunInvokeExpression::Operator::SET_EQUAL){
        auto arrayElementSize=Type::getSize((*ex->getArgs())[1]->getReturnType().get());
        addArraySetOpAsm(arrayElementSize);
        return;
    }

    addInstructionToConvertBetweenDataTypes(argSize, insideSize, isUnsigned);

    // TODO: optimize for imms

    switch(op){
        // args size is 0
        case OperatorFunInvokeExpression::Operator::UNARY_MINUS:{
            auto instructions=&currentAsmLabel->instructions;
            auto lastInstruction=&instructions->back();

            // Optimize If It's a negative imm
            if(lastInstruction->type!=Assembler::AsmInstruction::MOV){
                *currentAsmLabel+=Assembler::neg(Assembler::RAX());
                return;
            }
            auto lastSource=&lastInstruction->operands[1];
            if(lastSource->type==Assembler::AsmOperand::IMM)
                lastSource->value=L"-"+lastSource->value;
            return;

        }
        case OperatorFunInvokeExpression::Operator::LOGICAL_NOT:{
            *currentAsmLabel+=Assembler::_and(Assembler::RAX(), Assembler::imm(L"1"));
            *currentAsmLabel+=Assembler::_xor(Assembler::RAX(), Assembler::imm(L"1"));
            return;
        }
        case OperatorFunInvokeExpression::Operator::BIT_NOT:
            *currentAsmLabel+=Assembler::_not(Assembler::RAX());
            return;

        // args size is 1
        case OperatorFunInvokeExpression::Operator::PLUS:
        case OperatorFunInvokeExpression::Operator::MINUS:
        case OperatorFunInvokeExpression::Operator::TIMES:
        case OperatorFunInvokeExpression::Operator::DIV:
        case OperatorFunInvokeExpression::Operator::MOD:
        case OperatorFunInvokeExpression::Operator::XOR:
        case OperatorFunInvokeExpression::Operator::BIT_OR:
        case OperatorFunInvokeExpression::Operator::BIT_AND:
        case OperatorFunInvokeExpression::Operator::SHR:
        case OperatorFunInvokeExpression::Operator::SHL:
        case OperatorFunInvokeExpression::Operator::POW:
            ex->getFun()->accept(this);
            return;

        // args size is 2
        case OperatorFunInvokeExpression::Operator::SET_EQUAL:
        default:{}
    }

    *currentAsmLabel+=Assembler::pop(Assembler::RCX());  // pop (inside expression) into RCX

    auto greaterSize=(argSize>=insideSize)?argSize:insideSize;
    *currentAsmLabel+=Assembler::cmp(Assembler::RCX(greaterSize), Assembler::RAX(greaterSize));

    auto AL=Assembler::RAX(Assembler::AsmInstruction::BYTE);

    switch (op) {
        case OperatorFunInvokeExpression::Operator::EQUAL_EQUAL:
            *currentAsmLabel+=Assembler::setz(AL);
            break;
        case OperatorFunInvokeExpression::Operator::NOT_EQUAL:
            *currentAsmLabel+=Assembler::setnz(AL);
            break;
        case OperatorFunInvokeExpression::Operator::LESS:
            if(isUnsigned)
                *currentAsmLabel+=Assembler::setb(AL);
            else
                *currentAsmLabel+=Assembler::setl(AL);
            break;
        case OperatorFunInvokeExpression::Operator::LESS_EQUAL:
            if(isUnsigned)
                *currentAsmLabel+=Assembler::setbe(AL);
            else
                *currentAsmLabel+=Assembler::setle(AL);
            break;
        case OperatorFunInvokeExpression::Operator::GREATER:
            if(isUnsigned)
                *currentAsmLabel+=Assembler::seta(AL);
            else
                *currentAsmLabel+=Assembler::setg(AL);
            break;
        case OperatorFunInvokeExpression::Operator::GREATER_EQUAL:
            if(isUnsigned)
                *currentAsmLabel+=Assembler::setae(AL);
            else
                *currentAsmLabel+=Assembler::setge(AL);
            break;
        default:{}
    }

}

void Compiler::addInstructionToConvertBetweenDataTypes(int fromSize, int toSize, bool isUnsigned){
    
    if(toSize<=fromSize)
        return;

    if(isUnsigned){
        switch (fromSize) {
            case Assembler::AsmInstruction::BYTE:
                *currentAsmLabel+=Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFF"));
                return;
            case Assembler::AsmInstruction::WORD:
                *currentAsmLabel+=Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFFFF"));
                return;
            case Assembler::AsmInstruction::DWORD:
                *currentAsmLabel+=Assembler::mov(
                    Assembler::RAX(Assembler::AsmInstruction::DWORD),
                    Assembler::RAX(Assembler::AsmInstruction::DWORD)
                );
                return;
        }
    }

    switch (toSize) {
        case Assembler::AsmInstruction::QWORD:{
            switch (fromSize) {
                case Assembler::AsmInstruction::BYTE:
                    *currentAsmLabel+=Assembler::cbw();
                case Assembler::AsmInstruction::WORD:
                    *currentAsmLabel+=Assembler::cwde();
                case Assembler::AsmInstruction::DWORD:
                    *currentAsmLabel+=Assembler::cdqe();
            }
            break;
        }
        case Assembler::AsmInstruction::DWORD:{
            switch (fromSize) {
                case Assembler::AsmInstruction::BYTE:
                    *currentAsmLabel+=Assembler::cbw();
                case Assembler::AsmInstruction::WORD:
                    *currentAsmLabel+=Assembler::cwde();
            }
            break;
        }
        case Assembler::AsmInstruction::WORD:{
            switch (fromSize) {
                case Assembler::AsmInstruction::BYTE:
                    *currentAsmLabel+=Assembler::cbw();
            }
            break;
        }
    }
}

std::wstring Compiler::addAinAllocAsm(){

    if(AIN_ALLOC)
        return labelsAsm[AIN_ALLOC].label;

    auto decl=FunDecl(
        std::make_shared<std::wstring>(L"احجز"),
        Type::LONG,
        std::make_shared<bool>(false),
        std::make_shared<std::vector<SharedFunParam>>(
            std::vector{
                std::make_shared<FunParam>(
                    std::make_shared<std::wstring>(L"الحجم_بالبايت"),
                    Type::LONG
                )
            }
        )
    );

    AIN_ALLOC=PackageScope::AIN_PACKAGE
        ->findFileByPath(toWstring(BuiltInFilePaths::AIN_MEM))
        ->findPublicFunction(decl.toString())
        .get();

    if(!AIN_ALLOC)
        throw AinException(L"لم يتم العثور على الملف ainmem.ain في النظام.");

    if(labelsAsm.find(AIN_ALLOC)!=labelsAsm.end())
        return labelsAsm[AIN_ALLOC].label;

    AIN_ALLOC->accept(this);


    return labelsAsm[AIN_ALLOC].label;
}

std::wstring Compiler::addAinReAllocAsm(){

    if(AIN_REALLOC)
        return labelsAsm[AIN_REALLOC].label;

    auto decl=FunDecl(
        std::make_shared<std::wstring>(L"إعادة_حجز"),
        Type::LONG,
        std::make_shared<bool>(false),
        std::make_shared<std::vector<SharedFunParam>>(
            std::vector{
                std::make_shared<FunParam>(
                    std::make_shared<std::wstring>(L"العنوان"),
                    Type::LONG
                ),
                std::make_shared<FunParam>(
                    std::make_shared<std::wstring>(L"الحجم_الجديد_بالبايت"),
                    Type::LONG
                )
            }
        )
    );

    AIN_REALLOC=PackageScope::AIN_PACKAGE
        ->findFileByPath(toWstring(BuiltInFilePaths::AIN_MEM))
        ->findPublicFunction(decl.toString())
        .get();

    if(!AIN_REALLOC)
        throw AinException(L"لم يتم العثور على الملف ainmem.ain في النظام.");

    if(labelsAsm.find(AIN_REALLOC)!=labelsAsm.end())
        return labelsAsm[AIN_REALLOC].label;

    AIN_REALLOC->accept(this);
    
    return labelsAsm[AIN_REALLOC].label;
}

std::wstring Compiler::addAinMemcpyAsm(){

    if(AIN_MEMCPY)
        return labelsAsm[AIN_MEMCPY].label;

    auto decl=FunDecl(
        std::make_shared<std::wstring>(L"انسخ"),
        Type::LONG,
        std::make_shared<bool>(false),
        std::make_shared<std::vector<SharedFunParam>>(
            std::vector{
                std::make_shared<FunParam>(
                    std::make_shared<std::wstring>(L"من"),
                    Type::LONG
                ),
                std::make_shared<FunParam>(
                    std::make_shared<std::wstring>(L"إلى"),
                    Type::LONG
                ),
                std::make_shared<FunParam>(
                    std::make_shared<std::wstring>(L"العدد"),
                    Type::LONG
                )
            }
        )
    );

    AIN_MEMCPY=PackageScope::AIN_PACKAGE
        ->findFileByPath(toWstring(BuiltInFilePaths::AIN_MEM))
        ->findPublicFunction(decl.toString())
        .get();

    if(!AIN_MEMCPY)
        throw AinException(L"لم يتم العثور على الملف ainmem.ain في النظام.");

    if(labelsAsm.find(AIN_MEMCPY)!=labelsAsm.end())
        return labelsAsm[AIN_MEMCPY].label;

    AIN_MEMCPY->accept(this);

    return labelsAsm[AIN_MEMCPY].label;
}

std::wstring Compiler::addAinAllocateArrayAsm(){

    if(AIN_ALLOCATE_ARRAY)
        return labelsAsm[AIN_ALLOCATE_ARRAY].label;

    auto decl=FunDecl(
        std::make_shared<std::wstring>(L"احجز_مصفوفة"),
        Type::LONG,
        std::make_shared<bool>(false),
        std::make_shared<std::vector<SharedFunParam>>(
            std::vector{
                std::make_shared<FunParam>(
                    std::make_shared<std::wstring>(L"الأبعاد"),
                    std::make_shared<Type>(Type::Array(Type::LONG))
                ),
                std::make_shared<FunParam>(
                    std::make_shared<std::wstring>(L"حجم_العنصر"),
                    Type::LONG
                )
            }
        )
    );

    AIN_ALLOCATE_ARRAY=PackageScope::AIN_PACKAGE
        ->findFileByPath(toWstring(BuiltInFilePaths::AIN_MEM))
        ->findPrivateFunction(decl.toString())
        .get();

    if(!AIN_ALLOCATE_ARRAY)
        throw AinException(L"لم يتم العثور على الملف ainmem.ain في النظام.");

    if(labelsAsm.find(AIN_ALLOCATE_ARRAY)!=labelsAsm.end())
        return labelsAsm[AIN_ALLOCATE_ARRAY].label;

    AIN_ALLOCATE_ARRAY->accept(this);

    return labelsAsm[AIN_ALLOCATE_ARRAY].label;
}

void Compiler::callFunAsm(FunScope* fun, SharedVector<SharedIExpression> args, bool insideCall){

    auto params=fun->getParamsFromLocals();
    auto paramsDecl=fun->getDecl()->params;
    auto argsSize=getVariablesSize(params);

    if(insideCall){
        *currentAsmLabel+=Assembler::push(Assembler::RBX()); // The old address
        *currentAsmLabel+=Assembler::push(Assembler::RAX()); // inside ex address
    }

    *currentAsmLabel+=Assembler::reserveSpaceOnStack(argsSize); // for args

    auto offset=argsSize;

    for(auto i=0;i<args->size();i++){
        auto argEx=(*args)[i];
        argEx->accept(this);
        auto argSize=Type::getSize(argEx->getReturnType().get());
        offset-=argSize;
        auto comment=L"مُعامِل "+*(*paramsDecl)[i]->name;
        *currentAsmLabel+=
            Assembler::mov(
                Assembler::addressMov(Assembler::RSP(), offset),
                Assembler::RAX(argSize),
                Assembler::AsmInstruction::IMPLICIT,
                comment
            );
    }

    if (labelsAsm.find(fun)==labelsAsm.end())
        fun->accept(this);

    if (dynamic_cast<BuiltInFunScope*>(fun))
        return; // it will pop from the stack automaticaly

    auto decl=fun->getDecl();
    std::wstring comment=L"";

    if(auto parentClass=BaseScope::toClassScope(fun->getParentScope()))
        comment=L"استدعاء دالة "+parentClass->getName()+L"::"+decl->toString();
    else
        comment=L"استدعاء دالة "+decl->toString();

    if(insideCall)
        *currentAsmLabel+=Assembler::mov(
            Assembler::RBX(),
            Assembler::addressMov(Assembler::RSP(),argsSize)
        );
    
    *currentAsmLabel+=Assembler::call(
        Assembler::label(labelsAsm[fun].label),
        comment
    );

    if(insideCall)
        argsSize+=8; // remove more 8 byte for the size of inside ex address
    
    *currentAsmLabel+=Assembler::removeReservedSpaceFromStack(argsSize);

    if(insideCall)
        *currentAsmLabel+=Assembler::pop(Assembler::RBX()); // The old address

    // The returned address is on RAX
}

void Compiler::invokeNonStaticFun(NonStaticFunInvokeExpression* ex){
    ex->getInside()->accept(this);
    callFunAsm(
        ex->getFun().get(),
        ex->getArgs(),
        true
    );
}

void Compiler::invokeNonStaticBuiltInFun(NonStaticFunInvokeExpression* ex){
    auto opEx=OperatorFunInvokeExpression(
        0, // linenumber is not important here
        ex->getFunName(),
        ex->getArgs(),
        ex->getInside()
    );
    opEx.setFun(ex->getFun());
    opEx.setReturnType(ex->getReturnType());
    opEx.accept(this);
    switch (opEx.getOp()) {
        case OperatorFunInvokeExpression::Operator::NOT_EQUAL:
        case OperatorFunInvokeExpression::Operator::LESS:
        case OperatorFunInvokeExpression::Operator::LESS_EQUAL:
        case OperatorFunInvokeExpression::Operator::GREATER:
        case OperatorFunInvokeExpression::Operator::GREATER_EQUAL:
            currentAsmLabel->instructions.pop_back(); // remove the set instruction
            // FIXME: This it too long for this trivial code
            *currentAsmLabel+=Assembler::mov(Assembler::RDX(), Assembler::imm(L"0"));
            *currentAsmLabel+=Assembler::cmovz(Assembler::RAX(),Assembler::RDX());
            *currentAsmLabel+=Assembler::mov(Assembler::RDX(), Assembler::imm(L"-1"));
            *currentAsmLabel+=Assembler::cmovs(Assembler::RAX(),Assembler::RDX());
            *currentAsmLabel+=Assembler::mov(Assembler::RDX(), Assembler::imm(L"1"));
            *currentAsmLabel+=Assembler::cmovg(Assembler::RAX(),Assembler::RDX());
        default:
        break;
    }
}

void Compiler::invokeInsideString(NonStaticFunInvokeExpression* ex){
    auto args=ex->getArgs();
    auto fun=ex->getFun().get();

    // This is needed as the functions inside string may be not the same of th ex, as BuiltInFunScope adds the same functions for string for every file
    auto funInString=dynamic_cast<BuiltInFunScope*>(
        Type::STRING->getClassScope()->findPublicFunction(fun->getDecl()->toString()).get()
    );

    ex->getInside()->accept(this);
    for(auto arg:*args){
        *currentAsmLabel+=Assembler::push(Assembler::RAX()); // push (inside expression or the index arg for set operator) to the stack
        arg->accept(this);
    }

    // Last arg will be on RAX and the inside ex will be on the stack realtive to rsp

    if(fun->getName()==OperatorFunctions::GET_NAME){
        addArrayGetOpAsm(1); // To inline it
        return;
    }

    if(labelsAsm.find(funInString)==labelsAsm.end()){
        auto asmOfFun=funInString->getGeneratedAsm(this);
        labelsAsm[funInString].label=L"method"+std::to_wstring(++methodLabelsSize);
        labelsAsm[funInString].comment=L"دالة "+*Type::STRING_NAME+L"::"+funInString->getDecl()->toString();
        labelsAsm[funInString]+=asmOfFun;
    }

    *currentAsmLabel+=Assembler::call(
        Assembler::label(labelsAsm[funInString].label),
        L"استدعاء "+labelsAsm[funInString].comment
    );

}

void Compiler::leftAssign(IExpression* ex){
    if(dynamic_cast<VarAccessExpression*>(ex)||dynamic_cast<ThisVarAccessExpression*>(ex)){
        ex->accept(this);
        return;
    }
    if(auto varEx=dynamic_cast<NonStaticVarAccessExpression*>(ex)){
        varEx->getInside()->accept(this);
        *currentAsmLabel+=Assembler::push(Assembler::RAX());
        auto var=varEx->getVar().get();
        auto varSize=getVariableSize(var);
        auto offset=offsets[var];
        auto comment=
            ((*var->isValue())?L"الوصول لثابت ":L"الوصول لمتغير ")
            +varEx->getInside()->getReturnType()->getClassScope()->getName()
            +L"::"
            +*var->getName()
        ;

        *currentAsmLabel+=
            Assembler::mov(
                Assembler::RAX(varSize),
                Assembler::addressMov(Assembler::RAX(), offset.value),
                Assembler::AsmInstruction::IMPLICIT,
                comment
            )
        ;
        return;
    }
    throw("Not accessible");
}

void Compiler::rightAssign(IExpression* ex){
    if(auto varEx=dynamic_cast<VarAccessExpression*>(ex)){
        auto var=varEx->getVar().get();
        auto varSize=getVariableSize(var);
        auto offset=offsets[var];
        auto comment=L"تخصيص متغير "+*var->getName()+L": "+var->getType()->getClassScope()->getName();
        *currentAsmLabel+=Assembler::mov(
            Assembler::addressMov(offset.reg, offset.value),
            Assembler::RAX(varSize),
            Assembler::AsmInstruction::IMPLICIT,
            comment
        );
        return;
    }
    if(auto varEx=dynamic_cast<ThisVarAccessExpression*>(ex)){
        auto var=varEx->getVar().get();
        auto varSize=getVariableSize(var);
        auto offset=offsets[var];
        auto comment=L"تخصيص متغير "+*var->getName()+L": "+var->getType()->getClassScope()->getName();
        *currentAsmLabel+=Assembler::mov(
            Assembler::addressMov(offset.reg, offset.value),
            Assembler::RAX(varSize),
            Assembler::AsmInstruction::IMPLICIT,
            comment
        );
        return;
    }
    if(auto varEx=dynamic_cast<NonStaticVarAccessExpression*>(ex)){
        auto var=varEx->getVar().get();
        auto varSize=getVariableSize(var);
        auto offset=offsets[var];
        auto comment=L"تخصيص متغير "+*var->getName()+L": "+var->getType()->getClassScope()->getName();
        
        *currentAsmLabel+=Assembler::pop(Assembler::RDI()); // The address of inside ex
        *currentAsmLabel+=Assembler::mov(
            Assembler::addressMov(Assembler::RDI(), offset.value),
            Assembler::RAX(varSize),
            Assembler::AsmInstruction::IMPLICIT,
            comment
        );
        return;
    }
   throw("Not accessible");
}

void Compiler::addArrayGetOpAsm(int arrayElementSize){
    *currentAsmLabel+=Assembler::pop(Assembler::RDI()); // The array pointer which is first arg pushed as if get is a binary operator
    *currentAsmLabel+=Assembler::localLabel(L"if"+std::to_wstring(++currentIfLabelsSize));
    *currentAsmLabel+=Assembler::cmp(
        Assembler::RSI(),
        Assembler::addressMov(Assembler::RDI()),
        L"مقارنة رقم العنصر مع سعة المصفوفة"
    );
    auto endLabel=L"end"+std::to_wstring(currentIfLabelsSize);
    *currentAsmLabel+=Assembler::jl(Assembler::label(L"."+endLabel));
    // TODO: Throw exception asm:
    *currentAsmLabel+=Assembler::localLabel(endLabel);
    *currentAsmLabel+=Assembler::lea(
        Assembler::RSI(),
        Assembler::addressLea(
            Assembler::RDI().value+L"+8+"+Assembler::RAX().value+L"*"+std::to_wstring(arrayElementSize)
        ),
        Assembler::AsmInstruction::IMPLICIT,
        L"الوصول لرقم العنصر في المصفوفة"
    );
    *currentAsmLabel+=Assembler::mov(
        Assembler::RAX(arrayElementSize),
        Assembler::addressMov(Assembler::RSI()),
        Assembler::AsmInstruction::IMPLICIT,
        L"الوصول للعنصر في المصفوفة"
    );
}

void Compiler::addArraySetOpAsm(int arrayElementSize){
    *currentAsmLabel+=Assembler::pop(Assembler::RSI()); // The index arg
    *currentAsmLabel+=Assembler::pop(Assembler::RDI()); // The array pointer arg
    *currentAsmLabel+=Assembler::localLabel(L"if"+std::to_wstring(++currentIfLabelsSize));
    *currentAsmLabel+=Assembler::cmp(
        Assembler::RSI(),
        Assembler::addressMov(Assembler::RDI()),
        L"مقارنة رقم العنصر مع سعة المصفوفة"
    );
    auto endLabel=L"end"+std::to_wstring(currentIfLabelsSize);
    *currentAsmLabel+=Assembler::jl(Assembler::label(L"."+endLabel));
    // TODO: Throw exception asm:
    *currentAsmLabel+=Assembler::localLabel(endLabel);
    *currentAsmLabel+=Assembler::lea(
        Assembler::RSI(),
        Assembler::addressLea(
            Assembler::RDI().value+L"+8+"+Assembler::RSI().value+L"*"+std::to_wstring(arrayElementSize)
        ),
        Assembler::AsmInstruction::IMPLICIT,
        L"الوصول لرقم العنصر في المصفوفة"
    );
    *currentAsmLabel+=Assembler::mov(
        Assembler::addressMov(Assembler::RSI()),
        Assembler::RAX(arrayElementSize),
        Assembler::AsmInstruction::IMPLICIT,
        L"تخصيص العنصر في المصفوفة"
    );
}

void Compiler::compileBuiltInSetWithBuiltInAugOp(
    SetOperatorExpression::Operator op,
    int arrayElementSize,
    SharedIExpression valueEx,
    FunScope* funOfOp
){
    switch(op){
        case SetOperatorExpression::Operator::PLUS_EQUAL:
        case SetOperatorExpression::Operator::MINUS_EQUAL:
        case SetOperatorExpression::Operator::TIMES_EQUAL:
        case SetOperatorExpression::Operator::DIV_EQUAL:
        case SetOperatorExpression::Operator::MOD_EQUAL:
        case SetOperatorExpression::Operator::POW_EQUAL:
        case SetOperatorExpression::Operator::SHR_EQUAL:
        case SetOperatorExpression::Operator::SHL_EQUAL:
        case SetOperatorExpression::Operator::BIT_AND_EQUAL:
        case SetOperatorExpression::Operator::XOR_EQUAL:
        case SetOperatorExpression::Operator::BIT_OR_EQUAL:{
            *currentAsmLabel+=Assembler::push(Assembler::RAX()); // The value returned by get
            valueEx->accept(this);
            funOfOp->accept(this);
            *currentAsmLabel+=Assembler::mov(
                Assembler::addressMov(Assembler::RSI()),
                Assembler::RAX(arrayElementSize),
                Assembler::AsmInstruction::IMPLICIT,
                L"تخصيص العنصر في المصفوفة"
            );
            *currentAsmLabel+=Assembler::zero(Assembler::RAX());
            return;
        }
        case SetOperatorExpression::Operator::PRE_INC:{
            *currentAsmLabel+=Assembler::inc(
                Assembler::addressMov(Assembler::RSI()),
                Assembler::size(arrayElementSize)
            );
            *currentAsmLabel+=Assembler::mov(
                Assembler::RAX(),
                Assembler::addressMov(Assembler::RSI())
            );
            return;
        }
        case SetOperatorExpression::Operator::PRE_DEC:{
            *currentAsmLabel+=Assembler::dec(
                Assembler::addressMov(Assembler::RSI()),
                Assembler::size(arrayElementSize)
            );
            *currentAsmLabel+=Assembler::mov(
                Assembler::RAX(),
                Assembler::addressMov(Assembler::RSI())
            );
            return;
        }
        case SetOperatorExpression::Operator::POST_INC:{
            *currentAsmLabel+=Assembler::inc(
                Assembler::addressMov(Assembler::RSI()),
                Assembler::size(arrayElementSize)
            );
            return;
        }
        case SetOperatorExpression::Operator::POST_DEC:{
            *currentAsmLabel+=Assembler::dec(
                Assembler::addressMov(Assembler::RSI()),
                Assembler::size(arrayElementSize)
            );
            return;
        }
        default:{} // BAD_OP
    }
}

void Compiler::compileBuiltInSetWithNonBuiltInAugOp(
    SetOperatorExpression::Operator op,
    int arrayElementSize,
    SharedIExpression valueEx,
    FunScope* funOfOp
){
    *currentAsmLabel+=Assembler::push(Assembler::RSI()); // The address of the element in array
    switch(op){
        case SetOperatorExpression::Operator::PLUS_EQUAL:
        case SetOperatorExpression::Operator::MINUS_EQUAL:
        case SetOperatorExpression::Operator::TIMES_EQUAL:
        case SetOperatorExpression::Operator::DIV_EQUAL:
        case SetOperatorExpression::Operator::MOD_EQUAL:
        case SetOperatorExpression::Operator::POW_EQUAL:
        case SetOperatorExpression::Operator::SHR_EQUAL:
        case SetOperatorExpression::Operator::SHL_EQUAL:
        case SetOperatorExpression::Operator::BIT_AND_EQUAL:
        case SetOperatorExpression::Operator::XOR_EQUAL:
        case SetOperatorExpression::Operator::BIT_OR_EQUAL:{
            
            *currentAsmLabel+=Assembler::push(Assembler::RAX()); // The value returned by get
            valueEx->accept(this);
            *currentAsmLabel+=Assembler::pop(Assembler::RCX()); // The value returned by get

            *currentAsmLabel+=Assembler::push(Assembler::RBX()); // The prev object
            *currentAsmLabel+=Assembler::mov(Assembler::RBX(), Assembler::RCX()); // The value returned by get, i.e. the object address to preform op fun on it

            auto valueSize=Type::getSize(valueEx->getReturnType().get());
            if(valueSize!=8){
                *currentAsmLabel+=Assembler::reserveSpaceOnStack(valueSize);
                *currentAsmLabel+=Assembler::mov(
                    Assembler::addressMov(Assembler::RSP()),
                    Assembler::RAX(valueSize)
                );
            }
            else
                *currentAsmLabel+=Assembler::push(Assembler::RAX()); // The value to preform bin op on it, i.e. the arg of the op fun

            if(labelsAsm.find(funOfOp)==labelsAsm.end())
                funOfOp->accept(this);

            *currentAsmLabel+=Assembler::call(
                Assembler::label(labelsAsm[funOfOp].label),
                L"استدعاء دالة "+funOfOp->getParentScope()->getName()+L"::"+funOfOp->getDecl()->toString()
            );
            if(valueSize!=8)
                *currentAsmLabel+=Assembler::removeReservedSpaceFromStack(valueSize);
            else
                *currentAsmLabel+=Assembler::pop(Assembler::RDX()); // The value to preform bin op on it, i.e. the arg of the op fun

            *currentAsmLabel+=Assembler::pop(Assembler::RBX()); // The prev object
            break;
        }
        case SetOperatorExpression::Operator::POST_INC:
        case SetOperatorExpression::Operator::POST_DEC:
            *currentAsmLabel+=push(Assembler::RAX()); // The value returned by get
        
        case SetOperatorExpression::Operator::PRE_INC:
        case SetOperatorExpression::Operator::PRE_DEC:{
            *currentAsmLabel+=Assembler::push(Assembler::RBX()); // The prev object
            *currentAsmLabel+=Assembler::mov(Assembler::RBX(), Assembler::RAX()); // The value returned by get, i.e. the object address to preform op fun on it
            
            if(labelsAsm.find(funOfOp)==labelsAsm.end())
                funOfOp->accept(this);

            *currentAsmLabel+=Assembler::call(
                Assembler::label(labelsAsm[funOfOp].label),
                L"استدعاء دالة "+funOfOp->getParentScope()->getName()+L"::"+funOfOp->getDecl()->toString()
            );

            *currentAsmLabel+=Assembler::pop(Assembler::RBX()); // The prev object

            break;
        }
        default:{} // BAD_OP

        if(op==SetOperatorExpression::Operator::POST_INC||op==SetOperatorExpression::Operator::POST_DEC)
            *currentAsmLabel+=pop(Assembler::RAX()); // The value returned by get
        
    }

    *currentAsmLabel+=Assembler::pop(Assembler::RSI()); // The address of the element in array

    *currentAsmLabel+=Assembler::mov(
        Assembler::addressMov(Assembler::RSI()),
        Assembler::RAX(arrayElementSize),
        Assembler::AsmInstruction::IMPLICIT,
        L"تخصيص العنصر في المصفوفة"
    );

    switch (op) {
        case SetOperatorExpression::Operator::PRE_INC:
        case SetOperatorExpression::Operator::PRE_DEC:{
            *currentAsmLabel+=Assembler::mov(
                Assembler::RAX(),
                Assembler::addressMov(Assembler::RSI())
            );
            return;
        }
        case SetOperatorExpression::Operator::POST_INC:
        case SetOperatorExpression::Operator::POST_DEC:
            break;
        default:{
            *currentAsmLabel+=Assembler::zero(Assembler::RAX());
            return;
        }
    }
}