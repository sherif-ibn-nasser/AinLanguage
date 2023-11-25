#include "VarDecl.hpp"
#include "Type.hpp"

VarDecl::VarDecl(
    SharedWString name,
    SharedType type,
    SharedBool isVal
):name(name),type(type),isVal(isVal){}

bool VarDecl::operator==(const VarDecl& declaration)const{
    
    if(!(type&&declaration.type))
        return false;
    
    return
        *name==*declaration.name
        &&
        *type==*declaration.type
        &&
        *isVal==*declaration.isVal
    ;
}

bool VarDecl::operator!=(const VarDecl& declaration)const{
    return !(*this==declaration);
}

bool VarDecl::hasImplicitType()const {
    return this->type==nullptr;
}
