#include "Compiler.hpp"
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
#include "OperatorFunInvokeExpression.hpp"
#include "PackageScope.hpp"
#include "SharedPtrTypes.hpp"
#include "StmListScope.hpp"
#include "StringValue.hpp"
#include "Type.hpp"
#include "ULongValue.hpp"
#include "Variable.hpp"
#include "WhileStatement.hpp"
#include <cstddef>
#include <memory>
#include <string>

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
    // TODO    
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
        label=L"method"+std::to_wstring(methodLabelsSize);
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

    currentAsmLabel=&labelsAsm[scope];
    currentLoopLabelsSize=0;
    currentIfLabelsSize=0;

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
        *currentAsmLabel+=
            Assembler::mov(
                Assembler::RAX(),
                Assembler::addressMov(Assembler::RBX())
            );

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
    
}

void Compiler::visit(BuiltInFunScope* scope){
    *currentAsmLabel+=scope->getGeneratedAsm();
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
    /*
    * The left returns ts value in rax, also right, so remove last mov instruction for left, and swap the operands, to get more optimized code.
    * Fact: left is VarAccessExpression, NonStaticVarAccessExpression or ThisVarExpression
    * So left will generate last instruction in form of `mov RAX, [REG+offset]
    * And right will be any expression, that will return its value in RAX with the left of side
    * So just remove the last instruction of left, and add another one at the end, that has the form `mov [REG+offset], RAX
    * The first operand will be the value at the variable address, i.e, the second operand in the last instruction of left
    * the size of RAX in last instruction will match the size of the variable, i.e. the first operand in the last instruction of left
    */

    // constants cannot reach here

    stm->getLeft()->accept(this);
    auto instructions=&currentAsmLabel->instructions;
    auto lastInstruction=(*instructions)[instructions->size()-1];
    instructions->pop_back();
    
    stm->getRight()->accept(this);

    auto comment=lastInstruction.comment;
    std::wstring oldComment=L"الوصول ل";
    auto newComment=L"تخصيص ";
    comment.replace(0, oldComment.size(), newComment);

    *currentAsmLabel+=Assembler::mov(
        lastInstruction.operands[1],
        lastInstruction.operands[0],
        Assembler::AsmInstruction::IMPLICIT,
        comment
    );
}

void Compiler::visit(AugmentedAssignStatement* stm){
    if(auto isBuiltIn=std::dynamic_pointer_cast<BuiltInFunScope>(stm->getOpFun())){
        stm->getLeft()->accept(this);
        auto instructions=&currentAsmLabel->instructions;
        auto lastInstruction=(*instructions)[instructions->size()-1];
        
        *currentAsmLabel+=Assembler::push(Assembler::RAX());
        stm->getRight()->accept(this);
        stm->getOpFun()->accept(this);

        auto comment=lastInstruction.comment;
        std::wstring oldComment=L"الوصول ل";
        auto newComment=L"تخصيص ";
        comment.replace(0, oldComment.size(), newComment);

        *currentAsmLabel+=Assembler::mov(
            lastInstruction.operands[1],
            lastInstruction.operands[0],
            Assembler::AsmInstruction::IMPLICIT,
            comment
        );
    }else{
        // TODO
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

    auto fun=ex->getFun().get();
    auto args=ex->getArgs();
    auto params=fun->getParamsFromLocals();
    auto paramsDecl=fun->getDecl()->params;
    auto argsSize=getVariablesSize(params);

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
        ex->getFun()->accept(this);

    if (dynamic_cast<BuiltInFunScope*>(fun))
        return; // it will pop from the stack automaticaly

    auto funNameAsm=labelsAsm[fun].label;
    auto decl=fun->getDecl();
    std::wstring comment;

    if(auto parentClass=BaseScope::toClassScope(fun->getParentScope()))
        comment=L"استدعاء دالة "+parentClass->getName()+L"::"+decl->toString();
    else
        comment=L"استدعاء دالة "+decl->toString();

    *currentAsmLabel+=
        Assembler::call(
            Assembler::AsmOperand{.type=Assembler::AsmOperand::LABEL, .value=funNameAsm},
            comment
        );

    Assembler::removeReservedSpaceFromStack(argsSize);

    // The return value is stored in rax
}

void Compiler::visit(NewObjectExpression* ex){
    // TODO    
}

void Compiler::visit(NewArrayExpression* ex){
    // TODO    
}

void Compiler::visit(LiteralExpression* ex){
    // TODO: strings
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
        // TODO
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

    *currentAsmLabel+=Assembler::mov(Assembler::RBX(), Assembler::RAX());

    *currentAsmLabel+=
        Assembler::mov(
            Assembler::RAX(varSize),
            Assembler::addressMov(offset.reg, offset.value),
            Assembler::AsmInstruction::IMPLICIT,
            comment
        )
    ;
}

void Compiler::visit(NonStaticFunInvokeExpression* ex){
    // TODO    
}

void Compiler::visit(OperatorFunInvokeExpression* ex){
    if(auto builtIn=std::dynamic_pointer_cast<BuiltInFunScope>(ex->getFun())){
        invokeBuiltInOpFun(ex);
        return;
    }
}

void Compiler::visit(SetOperatorExpression* ex){
    // TODO    
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
    // TODO    
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
    auto size=Type::getSize(returnType);
    auto sizeAsm=Assembler::size(size);

    auto isPreInc=op==OperatorFunInvokeExpression::Operator::PRE_INC;
    auto isPreDec=op==OperatorFunInvokeExpression::Operator::PRE_DEC;
    auto isPostInc=op==OperatorFunInvokeExpression::Operator::POST_INC;
    auto isPostDec=op==OperatorFunInvokeExpression::Operator::POST_DEC;

    if(isPreInc||isPostInc||isPreDec||isPostDec){

        auto instructions=&currentAsmLabel->instructions;
        auto lastInstruction=instructions->back();

        auto comment=lastInstruction.comment;
        std::wstring oldComment=L"الوصول ل";
        auto newComment=(isPreInc||isPostInc)?L"زيادة ":L"نقصان ";
        comment.replace(0, oldComment.size(), newComment);

        if (isPreInc||isPreDec)
            instructions->pop_back();
        
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

        return;
    }

    for(auto arg:*ex->getArgs()){
        *currentAsmLabel+=Assembler::push(Assembler::RAX()); // push (inside expression) to the stack
        arg->accept(this);
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
        case OperatorFunInvokeExpression::Operator::GET:
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
    switch (toSize) {
        case Assembler::AsmInstruction::QWORD:{
            switch (fromSize) {
                case Assembler::AsmInstruction::BYTE:
                    *currentAsmLabel+=Assembler::cbw();
                case Assembler::AsmInstruction::WORD:
                    if(isUnsigned)
                        *currentAsmLabel+=Assembler::cwd();
                    else
                        *currentAsmLabel+=Assembler::cwde();
                case Assembler::AsmInstruction::DWORD:
                    if(isUnsigned)
                        *currentAsmLabel+=Assembler::cdq();
                    else
                        *currentAsmLabel+=Assembler::cdqe();
            }
            break;
        }
        case Assembler::AsmInstruction::DWORD:{
            switch (fromSize) {
                case Assembler::AsmInstruction::BYTE:
                    *currentAsmLabel+=Assembler::cbw();
                case Assembler::AsmInstruction::WORD:
                    if(isUnsigned)
                        *currentAsmLabel+=Assembler::cwd();
                    else
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