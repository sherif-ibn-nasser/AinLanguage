#include "IntValue.hpp"
#include "IntClassScope.hpp"
#include "Type.hpp"
#include <memory>

IntValue::IntValue(int value)
:IValue(Type::INT),PrimitiveValue(Type::INT,value){}

std::wstring IntValue::toString(){
    return std::to_wstring(value);
}