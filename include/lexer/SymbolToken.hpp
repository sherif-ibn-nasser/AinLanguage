#pragma once
#include <iostream>
#include "LexerToken.hpp"
class SymbolToken:public LexerToken
{
    
public:
    SymbolToken(std::wstring c);
    static SymbolToken LEFT_ANGLE_BRACKET;
    static SymbolToken RIGHT_ANGLE_BRACKET;
    static SymbolToken LEFT_PARENTHESIS;
    static SymbolToken RIGHT_PARENTHESIS;
    static SymbolToken LEFT_CURLY_BRACES;
    static SymbolToken RIGHT_CURLY_BRACES;
    static SymbolToken LEFT_SQUARE_BRACKET;
    static SymbolToken RIGHT_SQUARE_BRACKET;

    static SymbolToken COMMA;
    static SymbolToken COLON;
    static SymbolToken DOUBLE_COLONS;
    static SymbolToken SEMICOLON;
    static SymbolToken QUESTION_MARK;
    static SymbolToken EXCLAMATION_MARK;
    static SymbolToken BIT_NOT;
    static SymbolToken SHR;
    static SymbolToken SHL;
    static SymbolToken AMPERSAND;
    static SymbolToken XOR;
    static SymbolToken BAR;
    static SymbolToken DOT;
    static SymbolToken DOUBLE_QUOTE;
    static SymbolToken SINGLE_QUOTE;
    static SymbolToken BACK_SLASH;

    static SymbolToken PLUS;
    static SymbolToken MINUS;
    static SymbolToken STAR;
    static SymbolToken SLASH;
    static SymbolToken EQUAL;
    static SymbolToken MODULO;
    static SymbolToken POWER;

    static SymbolToken PLUS_PLUS;
    static SymbolToken MINUS_MINUS;

    static SymbolToken GREATER_EQUAL;
    static SymbolToken LESS_EQUAL;
    static SymbolToken EQUAL_EQUAL;
    static SymbolToken NOT_EQUAL;
    static SymbolToken LOGICAL_AND;
    static SymbolToken LOGICAL_OR;

    static SymbolToken PLUS_EQUAL;
    static SymbolToken MINUS_EQUAL;
    static SymbolToken STAR_EQUAL;
    static SymbolToken SLASH_EQUAL;
    static SymbolToken MODULO_EQUAL;
    static SymbolToken POWER_EQUAL;

    static SymbolToken SHR_EQUAL;
    static SymbolToken SHL_EQUAL;
    static SymbolToken BIT_NOT_EQUAL;
    static SymbolToken BIT_AND_EQUAL;
    static SymbolToken XOR_EQUAL;
    static SymbolToken BIT_OR_EQUAL;
};