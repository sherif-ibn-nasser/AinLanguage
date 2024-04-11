#include "ByteValue.hpp"
#include "ByteClassScope.hpp"
#include "Type.hpp"
#include <memory>

ByteValue::ByteValue(int value)
:IValue(Type::BYTE),PrimitiveValue(Type::BYTE,value){}

std::wstring ByteValue::toString(){
    return std::to_wstring(value);
}