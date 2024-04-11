#include "UByteValue.hpp"
#include "UByteClassScope.hpp"
#include "Type.hpp"
#include <memory>

UByteValue::UByteValue(unsigned int value)
:IValue(Type::UBYTE),PrimitiveValue(Type::UBYTE,value){}

std::wstring UByteValue::toString(){
    return std::to_wstring(value);
}