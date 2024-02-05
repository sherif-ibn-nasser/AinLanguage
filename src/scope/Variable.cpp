#include "Variable.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"
#include "VarDecl.hpp"
#include <memory>
#include <vector>

Variable::Variable(
    SharedWString name,
    SharedType type,
    SharedBool isVal
):
decl(std::make_shared<VarDecl>(name,type,isVal))
{}

Variable::Variable(SharedVarDecl decl):
decl(decl)
{}

SharedWString Variable::getName(){
    return this->decl->name;
}

SharedBool Variable::isValue(){
    return this->decl->isVal;
}

bool Variable::hasImplicitType(){
    return this->decl->hasImplicitType();
}

SharedType Variable::getType(){
    return this->decl->type;
}

void Variable::setType(SharedType type){
    this->decl->type=type;
}