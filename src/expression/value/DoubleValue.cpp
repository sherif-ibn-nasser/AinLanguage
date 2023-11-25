#include "DoubleValue.hpp"
#include "Type.hpp"

DoubleValue::DoubleValue(long double value)
:IValue(Type::DOUBLE),PrimitiveValue(Type::DOUBLE,value){}

std::wstring DoubleValue::toString(){
    return std::to_wstring(value);
}