#include "Compiler.hpp"
#include "BaseScope.hpp"
#include "BoolValue.hpp"
#include "CharValue.hpp"
#include "DoubleValue.hpp"
#include "FloatValue.hpp"
#include "FunDecl.hpp"
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
    auto parentClass=BaseScope::toClassScope(scope->getParentScope()); // may not exist if it's a global function
    auto isMain=scope->getName()==L"البداية";
    std::wstring labelName=L"";

    if (isConstructor)
        labelName=L"constructor"+std::to_wstring(++constructorLabelsSize);

    else if (parentClass)
        *currentLabelAsm+=L"method"+std::to_wstring(methodLabelsSize);

    else if (isMain)
        labelName=L"_start";

    else
        labelName=L"fun"+std::to_wstring(++funLabelsSize);
    
    labelsAsm[scope]={labelName,L""};
    auto prevLabelAsm=currentLabelAsm;
    currentLabelAsm=getAsmLabelInstructions(scope);
    *currentLabelAsm+=labelName+L":";
    
    // Add comment
    if (isConstructor)
        *currentLabelAsm+=L"\t; دالة "+decl->returnType->getClassScope()->getName()+L"::"+decl->toString()+L":\n";

    else if(parentClass)
        *currentLabelAsm+=L"\t; دالة "+parentClass->getName()+L"::"+decl->toString()+L":\n";

    else if (isMain)
        *currentLabelAsm+=L"\t; دالة البداية\n";

    else
        *currentLabelAsm+=L"\t; دالة "+scope->getDecl()->toString()+L"\n";

    *currentLabelAsm+=
        L"\tpush RBP\n"
        L"\tmov RBP, RSP\n"
    ;

    if(isConstructor)
        decl->returnType->getClassScope()->accept(this);

    // locals contains params, so we don't offset params
    auto nonParams=scope->getNonParamsFromLocals();
    int nonParamsReservedSize=0;

    if(nonParams->size()>0){
        nonParamsReservedSize=getVariablesSize(nonParams);
        reserveSpaceOnStack(nonParamsReservedSize);
    }

    for(auto stm:*scope->getStmList()){
        stm->accept(this);
    }

    if(isConstructor)
        *currentLabelAsm+=L"\tmov RAX, [RBX]\n";

    else if(*scope->getReturnType()==*Type::UNIT)
        *currentLabelAsm+=L"\txor RAX, RAX\n";

    *currentLabelAsm+=
        L"\tmov RSP, RBP\n"
        L"\tpop RBP\n"
    ;

    if(isMain)
        addExit0();
    else
        *currentLabelAsm+=L"\tret\n";

    currentLabelAsm=prevLabelAsm;
    
}

void Compiler::visit(BuiltInFunScope* scope){
    *currentLabelAsm+=scope->getGeneratedAsm();
}

void Compiler::visit(LoopScope* scope){
    // TODO    
}

void Compiler::visit(StmListScope* scope){
    // TODO    
}

void Compiler::visit(VarStm* stm){
    // TODO    
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
    // TODO    
}

void Compiler::visit(ExpressionStatement* stm){
    stm->getEx()->accept(this);
}

void Compiler::visit(VarAccessExpression* ex){
    // TODO    
}

void Compiler::visit(FunInvokeExpression* ex){

    auto fun=ex->getFun().get();
    auto args=ex->getArgs();
    auto params=fun->getParamsFromLocals();
    auto argsSize=getVariablesSize(params);

    reserveSpaceOnStack(argsSize); // for args

    auto offset=argsSize;

    for(auto argEx:*args){
        argEx->accept(this);
        auto argSize=Type::getSize(argEx->getReturnType().get());
        offset-=argSize;
        auto argOffsetStr=(offset==0)?L"":(L"+"+std::to_wstring(offset)); // the args are above rsp, so add L"+"
        *currentLabelAsm+=L"\tmov [RSP"+argOffsetStr+L"], "+getRaxBySize(argSize)+L"\n";
    }

    if (labelsAsm.find(fun)==labelsAsm.end()) {
        ex->getFun()->accept(this);
    }

    auto funNameAsm=getAsmLabelName(fun);

    *currentLabelAsm+=L"\tjmp "+funNameAsm;

    auto decl=fun->getDecl();

    if(auto parentClass=BaseScope::toClassScope(fun->getParentScope()))
        *currentLabelAsm+=L"\t; استدعاء دالة "+parentClass->getName()+L"::"+decl->toString()+L":\n";
    else
        *currentLabelAsm+=L"\t; استدعاء دالة "+decl->toString()+L"\n";

    removeReservedSpaceFromStack(argsSize);

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
    auto valAsm=getAsmValue(ex->getValue());
    *currentLabelAsm+=L"\tmov RAX, "+valAsm+L"\n";
}

void Compiler::visit(UnitExpression* ex){
    // TODO    
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
        asmFile+=labelAsmIt.second.second+L"\n";
    }
    return asmFile;
}

void Compiler::reserveSpaceOnStack(int size){
    *currentLabelAsm+=L"\tsub RSB, "+std::to_wstring(size)+L"\n";
}

void Compiler::removeReservedSpaceFromStack(int size){
    *currentLabelAsm+=L"\tadd RSB, "+std::to_wstring(size)+L"\n";
}

void Compiler::addExit(int errorCode){
    *currentLabelAsm+=
        L"\tmov RAX, 60\n"
        L"\tmov RDI, "+std::to_wstring(errorCode)+"\n"
        L"\tsyscall\t; إنهاء البرنامج\n"
    ;
}

void Compiler::addExit0(){
    *currentLabelAsm+=
        L"\tmov RAX, 60\n"
        L"\txor RDI, RDI\n"
        L"\tsyscall\t; إنهاء البرنامج\n"
    ;
}

int Compiler::getVariablesSize(SharedMap<std::wstring, SharedVariable> vars){
    auto size=0;
    for (auto varIt:*vars){
        size+=Type::getSize(varIt.second->getType().get());
    }
    return size;
}

std::wstring Compiler::getAsmValue(SharedIValue value){

    if(auto boolVal=std::dynamic_pointer_cast<BoolValue>(value))
        return (boolVal->getValue())?L"1":L"0";
    
    if(auto charVal=std::dynamic_pointer_cast<CharValue>(value))
        return L"\'"+charVal->toString()+L"\'";
    
    if(auto intVal=std::dynamic_pointer_cast<IntValue>(value)){
        return intVal->toString();
    }
    
    if(auto uintVal=std::dynamic_pointer_cast<UIntValue>(value)){
        return uintVal->toString();
    }

    if(auto longVal=std::dynamic_pointer_cast<LongValue>(value)){
        return longVal->toString();
    }
    
    if(auto ulongVal=std::dynamic_pointer_cast<ULongValue>(value)){
        return ulongVal->toString();
    }

    if(auto floatVal=std::dynamic_pointer_cast<FloatValue>(value)){
        union float_bytes {
            float fVal;
            int iVal;
        } data;
        data.fVal=floatVal->getValue();
        return std::to_wstring(data.iVal);
    }

    if(auto doubleVal=std::dynamic_pointer_cast<DoubleValue>(value)){
        union double_bytes {
            double dVal;
            long long lVal;
        } data;
        data.dVal=doubleVal->getValue();
        return std::to_wstring(data.lVal);
    }

    return L""; // TODO: strings

}

std::wstring Compiler::getAsmSize(int size){
    switch (size) {
        case 1: return L"BYTE";
        case 2: return L"WORD";
        case 4: return L"DWORD";
        default: return L"QWORD";
    }
}

std::wstring Compiler::getRaxBySize(int size){
    switch (size) {
        case 1: return L"AL";
        case 2: return L"AX";
        case 4: return L"EAX";
        default: return L"RAX";
    }
}

std::wstring Compiler::getAsmLabelName(StmListScope* scope){
    return labelsAsm[scope].first;
}

std::wstring* Compiler::getAsmLabelInstructions(StmListScope* scope){
    return &labelsAsm[scope].second;
}