#pragma once
#include <iostream>
#include "LiteralToken.hpp"

class NumberToken:public LiteralToken
{
    public:
        enum NUMBER_TYPE : int{
            BYTE=0,
            UNSIGNED_BYTE=1,
            INT=2,
            UNSIGNED_INT=3,
            LONG=4,
            UNSIGNED_LONG=5,
            FLOAT=6,
            DOUBLE=7,
        };
        NumberToken(NUMBER_TYPE numberType,std::wstring val);
        NUMBER_TYPE getNumberType();
    private:
        NUMBER_TYPE numberType;
        std::wstring val;
};