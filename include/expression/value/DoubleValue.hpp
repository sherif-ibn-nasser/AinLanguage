#pragma once
#include "PrimitiveValue.hpp"
class DoubleValue:public PrimitiveValue<double>{
    public:
        DoubleValue(double value);
        std::wstring toString()override;
};