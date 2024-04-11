#include "ShortValue.hpp"
#include "ShortClassScope.hpp"
#include "Type.hpp"
#include <memory>

ShortValue::ShortValue(int value)
:IValue(Type::SHORT),PrimitiveValue(Type::SHORT,value){}

std::wstring ShortValue::toString(){
    return std::to_wstring(value);
}