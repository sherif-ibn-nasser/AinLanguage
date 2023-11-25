#pragma once
#include <iostream>
#include "LiteralToken.hpp"

class NumberToken:public LiteralToken
{
    public:
        enum NUMBER_TYPE : int{
            INT=0,
            UNSIGNED_INT=1,
            LONG=2,
            UNSIGNED_LONG=3,
            DOUBLE=4,
            FLOAT=5,
        };
        NumberToken(NUMBER_TYPE numberType,std::wstring val);
        NUMBER_TYPE getNumberType();
    private:
        NUMBER_TYPE numberType;
        std::wstring val;
};