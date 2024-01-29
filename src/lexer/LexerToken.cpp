#include <iostream>
#include "LexerToken.hpp"
#include "KeywordToken.hpp"
#include "NumberToken.hpp"
#include "LiteralToken.hpp"

LexerToken::LexerToken(TOKEN_TYPE tokenType, std::wstring val):tokenType(tokenType),val(val){}

LexerToken::TOKEN_TYPE LexerToken::getTokenType(){
    return this->tokenType;
}

std::wstring LexerToken::getVal(){
    return this->val;
}

bool LexerToken::operator==(LexerToken &token) const{
    return
    this->tokenType==token.getTokenType()
    &&
    this->val==token.getVal();
}

bool LexerToken::operator!=(LexerToken &token) const{
    return !(*this==token);
}

bool LexerToken::isNumberLiteral(SharedLexerToken token){
    auto numToken=std::dynamic_pointer_cast<NumberToken>(token);
    return numToken!=nullptr;
}

bool LexerToken::isStringLiteral(SharedLexerToken token){
    auto stringToken=std::dynamic_pointer_cast<LiteralToken>(token);
    return stringToken!=nullptr && stringToken->getLiteralType()==LiteralToken::STRING;
}

std::wstring LexerToken::stringify(TOKEN_TYPE tokenType) {
    switch(tokenType){
        case BAD_TOKEN:return L"رمز غير صالح";
        case SYMBOL_TOKEN:return L"رمز";
        case LITERAL_TOKEN:return L"قيمة";
        case COMMENT_TOKEN:return L"تعليق";
        case KEYWORD_TOKEN:return L"كلمة مفتاحية";
        case IDENTIFIER_TOKEN:return L"مُعرِّف";
        case SPACE_TOKEN:return L"مسافة";
        case EOL_TOKEN:return L"نهاية السطر";
        case EOF_TOKEN:return L"نهاية الملف";
    }

    // to stop warning
    return L"";
}

LexerToken LexerToken::IdentifierToken(std::wstring val){
    return LexerToken(LexerToken::IDENTIFIER_TOKEN,val);
}

LexerToken LexerToken::SpaceToken(std::wstring val){
    return LexerToken(LexerToken::SPACE_TOKEN,val);
}

LexerToken LexerToken::EolToken() {
    return LexerToken(LexerToken::EOL_TOKEN,L"");
}

LexerToken LexerToken::EofToken() {
    return LexerToken(LexerToken::EOF_TOKEN,L"");
}

LexerToken::~LexerToken(){}