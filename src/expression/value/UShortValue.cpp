#include "UShortValue.hpp"
#include "UShortClassScope.hpp"
#include "Type.hpp"
#include <memory>

UShortValue::UShortValue(unsigned int value)
:IValue(Type::USHORT),PrimitiveValue(Type::USHORT,value){}

std::wstring UShortValue::toString(){
    return std::to_wstring(value);
}