#pragma once
#include <iostream>
#include "LexerToken.hpp"

class LiteralToken:public LexerToken
{
    public:
        enum LITERAL_TYPE : int{
            STRING=0,
            CHAR=1,
            NUMBER=2,
            BOOL=3
        };

        LiteralToken(LITERAL_TYPE literalType,std::wstring val);
        LITERAL_TYPE getLiteralType();
    private:
        LITERAL_TYPE literalType;
        std::wstring val;
};