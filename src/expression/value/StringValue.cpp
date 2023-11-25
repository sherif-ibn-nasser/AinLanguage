#include "StringValue.hpp"
#include "StringClassScope.hpp"
#include "Type.hpp"

StringValue::StringValue(std::wstring value)
:IValue(Type::STRING),value(value){}

std::wstring StringValue::toString(){
    return this->value;
}

std::wstring StringValue::getValue()const{
    return this->value;
}
