#pragma once
#include <iostream>
#include "LexerToken.hpp"
class KeywordToken:public LexerToken
{
    
public:
    KeywordToken(std::wstring val);
    static bool iskeyword(std::wstring val);
    static bool iskeyword(LexerToken t);
    
    static KeywordToken LET;
    static KeywordToken MUT;
    static KeywordToken OPERATOR;
    static KeywordToken FUN;
    static KeywordToken RETURN;

    static KeywordToken PACKAGE;
    static KeywordToken IMPORT;
    static KeywordToken NEW;
    static KeywordToken THIS;
    static KeywordToken CLASS;
    static KeywordToken INTERFACE;
    static KeywordToken OBJECT;

    static KeywordToken DATA;
    static KeywordToken ABSTRACT;
    static KeywordToken OPEN;
    static KeywordToken ENUM;
    static KeywordToken ANNOTATION;

    static KeywordToken PUBLIC;
    static KeywordToken PRIVATE;
    static KeywordToken PROTECTED;
    static KeywordToken OVERRIDE;

    static KeywordToken IF;
    //static KeywordToken ELSE_IF;
    static KeywordToken ELSE;
    static KeywordToken WHEN; // as switch

    static KeywordToken FOR;
    static KeywordToken DO;
    static KeywordToken WHILE;
    static KeywordToken BREAK;
    static KeywordToken CONTINUE;

    static KeywordToken THROW;
    static KeywordToken TRY;
    static KeywordToken CATCH;
    static KeywordToken FINALLY;
    
    // We may consider them as bool literal tokens, but we put them here instead
    static KeywordToken TRUE;
    static KeywordToken FALSE;
};