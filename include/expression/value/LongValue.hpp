#pragma once
#include "PrimitiveValue.hpp"
class LongValue:public PrimitiveValue<long long>{
    public:
        LongValue(long long value);
        std::wstring toString()override;
};