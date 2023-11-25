#include "VarsOffsetSetter.hpp"
#include "FunDecl.hpp"
#include "FunParam.hpp"
#include "Type.hpp"
#include "ArrayClassScope.hpp"

VarsOffsetSetter::Offset::Offset(int* reg, int value):
    reg(reg),
    value(value)
{}

VarsOffsetSetter::Offset::Offset():
    reg(0),
    value(0)
{}

VarsOffsetSetter::VarsOffsetSetter(
    std::unordered_map<Variable*, Offset>*offsets,
    int*const BP,
    int*const BX,
    const int*const DS
)
:offsets(offsets),
BP(BP),
BX(BX),
DS(DS)
{
    auto arrayCapacityProperty=Type::ARRAY_CLASS->getPublicVariables()->at(*ArrayClassScope::CAPACITY_NAME).get();
    (*offsets)[arrayCapacityProperty]=Offset(
        BX,
        0
    );
}

void VarsOffsetSetter::offsetStmListScope(StmListScope* scope){
    auto locals=scope->getLocals();

    for(auto varIt:*scope->getLocals()){
        
        (*offsets)[varIt.second.get()]=Offset(
            BP,
            stmListScopeOffset
        );

        stmListScopeOffset--;
    }

    for(auto stm:*scope->getStmList())
        stm->accept(this);
}

void VarsOffsetSetter::visit(PackageScope* scope){
    for(auto fileIterator:scope->getFiles()){
        fileIterator.second->accept(this);
    }
    for(auto packageIterator:scope->getPackages()){
        packageIterator.second->accept(this);
    }
}

void VarsOffsetSetter::visit(FileScope* scope){
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
    for(auto varIt:*scope->getPublicVariables()){
        auto var=varIt.second.get();
        (*offsets)[var]=Offset(
            const_cast<int*>(DS),
            globalVarsOffset
        );
        auto varType=var->getType()->getClassScope();
        globalVarsOffset+=varType->getPublicVariables()->size()+varType->getPrivateVariables()->size()+1;
    }
    for(auto varIt:*scope->getPrivateVariables()){
        auto var=varIt.second.get();
        (*offsets)[var]=Offset(
            const_cast<int*>(DS),
            globalVarsOffset
        );
        auto varType=var->getType()->getClassScope();
        globalVarsOffset+=varType->getPublicVariables()->size()+varType->getPrivateVariables()->size()+1;
    }

    /** TODO
    if(globalVarsOffset>=DATA_SIZE)
        throw;
    */
}

void VarsOffsetSetter::visit(ClassScope* scope){
    auto varsOffset=1;
    for(auto varIt:*scope->getPublicVariables()){
        (*offsets)[varIt.second.get()]=Offset(
            BX,
            varsOffset
        );

        varsOffset++;
    }
    for(auto varIt:*scope->getPrivateVariables()){
        (*offsets)[varIt.second.get()]=Offset(
            BX,
            varsOffset
        );

        varsOffset++;
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

void VarsOffsetSetter::visit(FunScope* scope){
    auto params=scope->getDecl()->params;

    auto paramsOffsets=std::unordered_map<std::wstring,int>{};

    auto offset=1;
    for(auto paramIt=params->rbegin();paramIt!=params->rend();paramIt++){

        paramsOffsets[
            *paramIt->get()->name
        ]=offset++;

    }

    stmListScopeOffset=-1;
    for(auto varIt:*scope->getLocals()){
        
        auto paramOffsetIt=paramsOffsets.find(varIt.first);
        
        (*offsets)[varIt.second.get()]=Offset(
            BP,
            (paramOffsetIt==paramsOffsets.end())
            ?stmListScopeOffset
            :paramOffsetIt->second
        );

        if(paramOffsetIt==paramsOffsets.end())
            stmListScopeOffset--;
    }

    for(auto stm:*scope->getStmList())
        stm->accept(this);
}

void VarsOffsetSetter::visit(IfStatement* stm){
    offsetStmListScope(stm->getIfScope().get());

    if(auto elseScope=stm->getElseScope().get())
        offsetStmListScope(elseScope);
}

void VarsOffsetSetter::visit(WhileStatement* stm){
    offsetStmListScope(stm->getLoopScope().get());
}

void VarsOffsetSetter::visit(DoWhileStatement* stm){
    offsetStmListScope(stm->getLoopScope().get());
}