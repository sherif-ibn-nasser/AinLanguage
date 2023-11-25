#include "SymbolToken.hpp"

SymbolToken::SymbolToken(std::wstring c):
LexerToken(LexerToken::SYMBOL_TOKEN,c){}

SymbolToken SymbolToken::LEFT_ANGLE_BRACKET=SymbolToken(L"<");
SymbolToken SymbolToken::RIGHT_ANGLE_BRACKET=SymbolToken(L">");
SymbolToken SymbolToken::LEFT_PARENTHESIS=SymbolToken(L"(");
SymbolToken SymbolToken::RIGHT_PARENTHESIS=SymbolToken(L")");
SymbolToken SymbolToken::LEFT_CURLY_BRACES=SymbolToken(L"{");
SymbolToken SymbolToken::RIGHT_CURLY_BRACES=SymbolToken(L"}");
SymbolToken SymbolToken::LEFT_SQUARE_BRACKET=SymbolToken(L"[");
SymbolToken SymbolToken::RIGHT_SQUARE_BRACKET=SymbolToken(L"]");
SymbolToken SymbolToken::COMMA=SymbolToken(L"،");
SymbolToken SymbolToken::COLON=SymbolToken(L":");
SymbolToken SymbolToken::DOUBLE_COLONS=SymbolToken(L"::");
SymbolToken SymbolToken::SEMICOLON=SymbolToken(L"؛");

SymbolToken SymbolToken::QUESTION_MARK=SymbolToken(L"؟");
SymbolToken SymbolToken::EXCLAMATION_MARK=SymbolToken(L"!");
SymbolToken SymbolToken::BIT_NOT=SymbolToken(L"~");
SymbolToken SymbolToken::SHR=SymbolToken(L"<<");
SymbolToken SymbolToken::SHL=SymbolToken(L">>");
SymbolToken SymbolToken::AMPERSAND=SymbolToken(L"&");
SymbolToken SymbolToken::XOR=SymbolToken(L"^");
SymbolToken SymbolToken::BAR=SymbolToken(L"|");
SymbolToken SymbolToken::DOT=SymbolToken(L".");
SymbolToken SymbolToken::DOUBLE_QUOTE=SymbolToken(L"\"");
SymbolToken SymbolToken::SINGLE_QUOTE=SymbolToken(L"\"");
SymbolToken SymbolToken::BACK_SLASH=SymbolToken(L"\\");

SymbolToken SymbolToken::PLUS=SymbolToken(L"+");
SymbolToken SymbolToken::MINUS=SymbolToken(L"-");
SymbolToken SymbolToken::STAR=SymbolToken(L"*");
SymbolToken SymbolToken::SLASH=SymbolToken(L"/");
SymbolToken SymbolToken::EQUAL=SymbolToken(L"=");
SymbolToken SymbolToken::MODULO=SymbolToken(L"%");
SymbolToken SymbolToken::POWER=SymbolToken(L"**");

SymbolToken SymbolToken::PLUS_PLUS=SymbolToken(L"++");
SymbolToken SymbolToken::MINUS_MINUS=SymbolToken(L"--");

SymbolToken SymbolToken::GREATER_EQUAL=SymbolToken(L">=");
SymbolToken SymbolToken::LESS_EQUAL=SymbolToken(L"<=");
SymbolToken SymbolToken::EQUAL_EQUAL=SymbolToken(L"==");
SymbolToken SymbolToken::NOT_EQUAL=SymbolToken(L"!=");
SymbolToken SymbolToken::LOGICAL_AND=SymbolToken(L"&&");
SymbolToken SymbolToken::LOGICAL_OR=SymbolToken(L"||");

SymbolToken SymbolToken::PLUS_EQUAL=SymbolToken(L"+=");
SymbolToken SymbolToken::MINUS_EQUAL=SymbolToken(L"-=");
SymbolToken SymbolToken::STAR_EQUAL=SymbolToken(L"*=");
SymbolToken SymbolToken::SLASH_EQUAL=SymbolToken(L"/=");
SymbolToken SymbolToken::MODULO_EQUAL=SymbolToken(L"%=");
SymbolToken SymbolToken::POWER_EQUAL=SymbolToken(L"**=");

SymbolToken SymbolToken::SHR_EQUAL=SymbolToken(L"<<=");
SymbolToken SymbolToken::SHL_EQUAL=SymbolToken(L">>=");
SymbolToken SymbolToken::BIT_NOT_EQUAL=SymbolToken(L"~=");
SymbolToken SymbolToken::BIT_AND_EQUAL=SymbolToken(L"&=");
SymbolToken SymbolToken::XOR_EQUAL=SymbolToken(L"^=");
SymbolToken SymbolToken::BIT_OR_EQUAL=SymbolToken(L"|=");