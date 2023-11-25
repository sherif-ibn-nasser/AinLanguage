#include "FloatValue.hpp"
#include "Type.hpp"

FloatValue::FloatValue(float value)
:IValue(Type::FLOAT),PrimitiveValue(Type::FLOAT,value){}

std::wstring FloatValue::toString(){
    return std::to_wstring(value);
}