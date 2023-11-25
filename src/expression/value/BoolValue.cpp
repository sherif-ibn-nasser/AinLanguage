#include "BoolValue.hpp"
#include "KeywordToken.hpp"
#include "Type.hpp"

BoolValue::BoolValue(bool value)
:IValue(Type::BOOL),PrimitiveValue(Type::BOOL,value){}

std::wstring BoolValue::toString(){
    if(value)
        return KeywordToken::TRUE.getVal();
    return KeywordToken::FALSE.getVal();
}
