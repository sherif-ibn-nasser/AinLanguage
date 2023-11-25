#include "ULongValue.hpp"
#include "Type.hpp"

ULongValue::ULongValue(unsigned long long value)
:IValue(Type::ULONG),PrimitiveValue(Type::ULONG,value){}

std::wstring ULongValue::toString(){
    return std::to_wstring(value);
}