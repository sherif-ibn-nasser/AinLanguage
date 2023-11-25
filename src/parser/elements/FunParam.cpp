#include "FunParam.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"

FunParam::FunParam(SharedWString name, SharedType type)
:name(name),type(type){}

bool FunParam::operator==(const FunParam& param)const{
    return 
        *this->name==*param.name
        &&
        *this->type==*param.type
    ;
}

bool FunParam::operator!=(const FunParam& param)const{
    return !(*this==param);
}
