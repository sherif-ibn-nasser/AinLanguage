#include "Compiler.hpp"
#include "FunDecl.hpp"
#include "Type.hpp"
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
    auto isMain=scope->getName()==L"البداية";

    labelsAsm[scope]=L"";

    auto labelAsm=&labelsAsm[scope];

    if (isConstructor)
        *labelAsm+=
            L"\n; دالة إنشاء "+decl->returnType->getClassScope()->getName()+
            L"\nconstructor"+std::to_wstring(labelsAsm.size())+L":\n"
        ;
    
    else if (isMain)
        *labelAsm+=
            L"\n; دالة البداية\n"
            L"_start:\n"
        ;

    else
        *labelAsm+=
            L"\n; دالة "+scope->getName()+L"\n"
            L"fun"+std::to_wstring(labelsAsm.size())+L":\n"
        ;

    *labelAsm+=
        L"  push RBP\n"
        L"  mov RBP, RSP\n"
    ;

    if(isConstructor)
        decl->returnType->getClassScope()->accept(this);

    // locals contains params, so we don't offset params
    auto localsSize=scope->getLocals()->size();
    auto paramsSize=scope->getDecl()->params->size();

    if(localsSize>paramsSize)
        reserveSpaceForStmListLocals(labelAsm,localsSize-paramsSize);

    for(auto stm:*scope->getStmList()){
        stm->accept(this);
    }


    if(isConstructor)
        *labelAsm+=L"  mov RAX, [RBX]\n";

    else if(*scope->getReturnType()==*Type::UNIT)
        *labelAsm+=L"  xor RAX, RAX\n";

    *labelAsm+=
        L"  mov RSP, RBP\n"
        L"  pop RBP\n"
    ;

    if(isMain)
        addExit(labelAsm, 0);
    else
        *labelAsm+=L"   ret";
    
    
}

void Compiler::visit(BuiltInFunScope* scope){
    // TODO    
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
    // TODO    
}

void Compiler::visit(VarAccessExpression* ex){
    // TODO    
}

void Compiler::visit(FunInvokeExpression* ex){
    // TODO    
}

void Compiler::visit(NewObjectExpression* ex){
    // TODO    
}

void Compiler::visit(NewArrayExpression* ex){
    // TODO    
}

void Compiler::visit(LiteralExpression* ex){
    // TODO    
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
        asmFile+=labelAsmIt.second;
    }
    return asmFile;
}

void Compiler::reserveSpaceForStmListLocals(std::wstring* labelAsm,int size){
    *labelAsm+=L"   sub RSB,"+std::to_wstring(size)+L"\n";
}

void Compiler::removeReservedSpaceForStmListLocals(std::wstring* labelAsm,int size){
    *labelAsm+=L"   add RSB,"+std::to_wstring(size)+L"\n";
}

void Compiler::addExit(std::wstring* labelAsm, int errorCode){
    *labelAsm+=
            L"\n  ; إنهاء البرنامج\n"
            L"  mov RAX, 60\n"
            L"  mov RDI, "+std::to_wstring(errorCode)+"\n"
            L"  syscall\n"
        ;
}