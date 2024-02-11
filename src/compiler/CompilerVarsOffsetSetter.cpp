#include "CompilerVarsOffsetSetter.hpp"
#include "Assembler.hpp"
#include "ClassScope.hpp"
#include "LoopScope.hpp"
#include "FunDecl.hpp"
#include "FunParam.hpp"
#include "Type.hpp"
#include "ArrayClassScope.hpp"
#include "Variable.hpp"
#include <string>

CompilerVarsOffsetSetter::Offset::Offset(Assembler::AsmOperand reg, int value):
    reg(reg),
    value(value)
{}

CompilerVarsOffsetSetter::Offset::Offset():
    reg({}),
    value(0)
{}

CompilerVarsOffsetSetter::CompilerVarsOffsetSetter(
    std::unordered_map<Variable*, Offset>*offsets
)
:offsets(offsets)
{
    auto arrayCapacityProperty=Type::ARRAY_CLASS->getPublicVariables()->at(*ArrayClassScope::CAPACITY_NAME).get();
    (*offsets)[arrayCapacityProperty]=Offset(
        Assembler::RBX(),
        0
    );
}

void CompilerVarsOffsetSetter::offsetStmListScope(StmListScope* scope){
    auto locals=scope->getLocals();

    for(auto varIt:*scope->getLocals()){

        auto var=varIt.second.get();

        stmListScopeOffset-=Type::getSize(var->getType().get());
        
        (*offsets)[var]=Offset(
            Assembler::RBP(),
            stmListScopeOffset
        );

    }

    for(auto stm:*scope->getStmList())
        stm->accept(this);
}

void CompilerVarsOffsetSetter::visit(PackageScope* scope){
    for(auto fileIterator:scope->getFiles()){
        fileIterator.second->accept(this);
    }
    for(auto packageIterator:scope->getPackages()){
        packageIterator.second->accept(this);
    }
}

void CompilerVarsOffsetSetter::visit(FileScope* scope){
    for(auto funIt:*scope->getPublicFunctions()){
        funIt.second->accept(this);
    }
    for(auto funIt:*scope->getPrivateFunctions()){
        funIt.second->accept(this);
    }
    for(auto funIt:*scope->getPublicClasses()){
        funIt.second->accept(this);
    }
    for(auto funIt:*scope->getPrivateClasses()){
        funIt.second->accept(this);
    }

    // FIXME: how to store global variables and constants
    /*
    for(auto varIt:*scope->getPublicVariables()){
        auto var=varIt.second.get();
        (*offsets)[var]=Offset(
            Compiler::RDS,
            globalVarsOffset
        );
        globalVarsOffset+=var->getType()->getClassScope()->getSize()+8; // + 8 bytes for the address
    }

    for(auto varIt:*scope->getPrivateVariables()){
        auto var=varIt.second.get();
        (*offsets)[var]=Offset(
            Compiler::RDS,
            globalVarsOffset
        );
        globalVarsOffset+=var->getType()->getClassScope()->getSize()+8; // + 8 bytes for the address
    }
    */

    /** TODO
    if(globalVarsOffset>=DATA_SIZE)
        throw;
    */
}

void CompilerVarsOffsetSetter::visit(ClassScope* scope){

    auto varsOffset=8; // for first offset before 8-byte RBX register, TODO: need to handle if the system is 32-bit

    for(auto varIt:*scope->getPublicVariables()){
        auto var=varIt.second.get();

        (*offsets)[var]=Offset(
            Assembler::RBX(),
            varsOffset
        );

        varsOffset+=Type::getSize(var->getType().get());
    }

    for(auto varIt:*scope->getPrivateVariables()){
        auto var=varIt.second.get();

        (*offsets)[var]=Offset(
            Assembler::RBX(),
            varsOffset
        );

        varsOffset+=Type::getSize(var->getType().get());
    }

    for(auto constructorIt:*scope->getPublicConstructors()){
        constructorIt.second->accept(this);
    }
    for(auto constructorIt:*scope->getPrivateConstructors()){
        constructorIt.second->accept(this);
    }
    
    for(auto funIt:*scope->getPublicFunctions()){
        funIt.second->accept(this);
    }
    for(auto funIt:*scope->getPrivateFunctions()){
        funIt.second->accept(this);
    }
}

void CompilerVarsOffsetSetter::visit(FunScope* scope){
    auto paramsVec=scope->getDecl()->params;

    auto locals=scope->getLocals();

    auto nonParams=scope->getNonParamsFromLocals();

    stmListScopeOffset=16; // for first offset before 8-byte RBP register and return address, TODO: need to handle if the system is 32-bit

    for(auto paramIt=paramsVec->rbegin();paramIt!=paramsVec->rend();paramIt++){

        auto name=*paramIt->get()->name;
        auto var=(*locals)[name].get();

        (*offsets)[var]=Offset(
            Assembler::RBP(),
            stmListScopeOffset
        );

        stmListScopeOffset+=Type::getSize(var->getType().get());
    }

    stmListScopeOffset=0; // for first offset after 8-byte RSP register, the offset will be decreased by the variable size

    for(auto varIt:*nonParams){

        auto var=varIt.second.get();
        
        stmListScopeOffset-=Type::getSize(var->getType().get());

        (*offsets)[var]=Offset(
            Assembler::RBP(),
            stmListScopeOffset
        );

    }

    for(auto stm:*scope->getStmList())
        stm->accept(this);
}

void CompilerVarsOffsetSetter::visit(IfStatement* stm){
    offsetStmListScope(stm->getIfScope().get());

    if(auto elseScope=stm->getElseScope().get())
        offsetStmListScope(elseScope);
}

void CompilerVarsOffsetSetter::visit(WhileStatement* stm){
    stmListScopeOffset-=8; // To save the continue address of the loop in a register
    offsetStmListScope(stm->getLoopScope().get());
    stmListScopeOffset+=8; // Restore save register
}

void CompilerVarsOffsetSetter::visit(DoWhileStatement* stm){
    stmListScopeOffset-=8; // To save the continue address of the loop in a register
    offsetStmListScope(stm->getLoopScope().get());
    stmListScopeOffset+=8; // Restore save register
}