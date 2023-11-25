#include "LiteralToken.hpp"

LiteralToken::LiteralToken(LITERAL_TYPE literalType,std::wstring val)
:LexerToken(LexerToken::LITERAL_TOKEN,val),literalType(literalType){}

LiteralToken::LITERAL_TYPE LiteralToken::getLiteralType(){
    return literalType;
}