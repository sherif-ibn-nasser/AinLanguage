#pragma once
#include "PrimitiveValue.hpp"
class ULongValue:public PrimitiveValue<unsigned long long>{
    public:
        ULongValue(unsigned long long value);
        std::wstring toString()override;
};