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
#include "IntValue.hpp"
#include "LongValue.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"
#include "ULongValue.hpp"
#include <memory>
#include <string>

void Compiler::visit(PackageScope* packageScope){
    // TODO
}

void Compiler::visit(FileScope* scope){
    // TODO    
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
    currentAsmLabel=getAsmLabelInstructions(scope);


    *currentAsmLabel+=Assembler::push(Assembler::RBP());
    *currentAsmLabel+=Assembler::mov(Assembler::RBP(), Assembler::RSP());

    if(isConstructor)
        decl->returnType->getClassScope()->accept(this);

    // locals contains params, so we don't offset params
    auto nonParams=scope->getNonParamsFromLocals();
    int nonParamsReservedSize=0;

    if(nonParams->size()>0){
        nonParamsReservedSize=getVariablesSize(nonParams);
        Assembler::reserveSpaceOnStack(nonParamsReservedSize);
    }

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
    
    if(isMain)
        *currentAsmLabel+=Assembler::exit(0);
    else
        *currentAsmLabel+=Assembler::ret();

    currentAsmLabel=prevLabelAsm;
    
}

void Compiler::visit(BuiltInFunScope* scope){
    *currentAsmLabel+=scope->getGeneratedAsm();
}

void Compiler::visit(LoopScope* scope){
    // TODO    
}

void Compiler::visit(StmListScope* scope){
    // TODO    
}

void Compiler::visit(VarStm* stm){
    auto var=stm->getVar();
    auto varSize=getVariableSize(var);
    auto offset=offsets[var.get()];
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
    // TODO    
}

void Compiler::visit(AugmentedAssignStatement* stm){
    // TODO    
}

void Compiler::visit(IfStatement* stm){
    // TODO    
}

void Compiler::visit(WhileStatement* stm){
    // TODO    
}

void Compiler::visit(DoWhileStatement* stm){
    // TODO    
}

void Compiler::visit(BreakStatement* stm){
    // TODO    
}

void Compiler::visit(ContinueStatement* stm){
    // TODO    
}

void Compiler::visit(ReturnStatement* stm){
    auto fun=BaseScope::getContainingFun(stm->getRunScope()).get();

    stm->getEx()->accept(this);

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
    auto var=ex->getVar();
    auto varSize=getVariableSize(var);
    auto offset=offsets[var.get()];
    auto comment=((*var->isValue())?L"الوصول لثابت ":L"الوصول لمتغير ")+*var->getName();
    *currentAsmLabel+=
        Assembler::mov(
            Assembler::RAX(varSize),
            Assembler::addressMov(offset.reg, offset.value),
            Assembler::AsmInstruction::IMPLICIT,
            comment
        );
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

    auto funNameAsm=getAsmLabelName(fun);
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
    if(auto boolVal=std::dynamic_pointer_cast<BoolValue>(value))
        imm=(boolVal->getValue())
            ?Assembler::imm(L"1")
            :Assembler::imm(L"0");
    
    else if(auto charVal=std::dynamic_pointer_cast<CharValue>(value)){
        auto val=charVal->toString();
        if(val==L"\n")
            imm=Assembler::imm(L"0x0a");
        else
            imm=Assembler::imm(L"\'"+val+L"\'");
    }
    
    else if(auto intVal=std::dynamic_pointer_cast<IntValue>(value)){
        imm=Assembler::imm(intVal->toString());
    }
    
    else if(auto uintVal=std::dynamic_pointer_cast<UIntValue>(value)){
        imm=Assembler::imm(uintVal->toString());
    }

    else if(auto longVal=std::dynamic_pointer_cast<LongValue>(value)){
        imm=Assembler::imm(longVal->toString());
    }
    
    else if(auto ulongVal=std::dynamic_pointer_cast<ULongValue>(value)){
        imm=Assembler::imm(ulongVal->toString());
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

    // TODO: strings

    *currentAsmLabel+=Assembler::mov(Assembler::RAX(),imm);
}

void Compiler::visit(UnitExpression* ex){
    *currentAsmLabel+=Assembler::zero(Assembler::RAX());
}

void Compiler::visit(LogicalExpression* ex){
    // TODO    
}

void Compiler::visit(NonStaticVarAccessExpression* ex){
    // TODO    
}

void Compiler::visit(NonStaticFunInvokeExpression* ex){
    // TODO    
}

void Compiler::visit(OperatorFunInvokeExpression* ex){
    // TODO    
}

void Compiler::visit(SetOperatorExpression* ex){
    // TODO    
}

void Compiler::visit(ThisExpression* ex){
    // TODO    
}

void Compiler::visit(ThisVarAccessExpression* ex){
    // TODO    
}

void Compiler::visit(ThisFunInvokeExpression* ex){
    // TODO    
}

std::wstring Compiler::getAssemblyFile(){
    auto asmFile=dataAsm+bssAsm+textAsm+L"\n";
    for(auto labelAsmIt:labelsAsm){
        asmFile+=labelAsmIt.second.getAsmText()+L"\n";
    }
    return asmFile;
}

int Compiler::getVariableSize(SharedVariable var){
    return Type::getSize(var->getType().get());
}

int Compiler::getVariablesSize(SharedMap<std::wstring, SharedVariable> vars){
    auto size=0;
    for (auto varIt:*vars){
        size+=getVariableSize(varIt.second);
    }
    return size;
}

std::wstring Compiler::getAsmLabelName(StmListScope* scope){
    return labelsAsm[scope].label;
}

Assembler::AsmLabel* Compiler::getAsmLabelInstructions(StmListScope* scope){
    return &labelsAsm[scope];
}