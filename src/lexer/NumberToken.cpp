#include "NumberToken.hpp"

NumberToken::NumberToken(NUMBER_TYPE numberType,std::wstring val)
:LiteralToken(LiteralToken::NUMBER,val),numberType(numberType){}

NumberToken::NUMBER_TYPE NumberToken::getNumberType(){
    return numberType;
}