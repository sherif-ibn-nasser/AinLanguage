#pragma once
#include <iostream>
#include "LiteralToken.hpp"

class NumberToken:public LiteralToken
{
    public:
        enum NUMBER_TYPE : int{
            BYTE=0,
            UNSIGNED_BYTE=1,
            SHORT=2,
            UNSIGNED_SHORT=3,
            INT=4,
            UNSIGNED_INT=5,
            LONG=6,
            UNSIGNED_LONG=7,
            FLOAT=8,
            DOUBLE=9,
        };
        NumberToken(NUMBER_TYPE numberType,std::wstring val);
        NUMBER_TYPE getNumberType();
    private:
        NUMBER_TYPE numberType;
        std::wstring val;
};