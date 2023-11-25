#include "SharedPtrTypes.hpp"
#include "IValue.hpp"

IValue::IValue(SharedType type):type(type){}

SharedType IValue::getType(){
    return this->type;
}

IValue::~IValue(){}