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

int Variable::getSize(){

    auto type=*getType();

    if (type==*Type::BOOL)
        return 1;
    
    if (type==*Type::CHAR)
        return 2;

    if (type==*Type::FLOAT||type==*Type::INT||type==*Type::UINT)
        return 4;

    if (type==*Type::DOUBLE||type==*Type::LONG||type==*Type::ULONG)
        return 8;

    // addresses are 8 bytes, TODO: need to handle if the system is 32-bit
    return 8;
}