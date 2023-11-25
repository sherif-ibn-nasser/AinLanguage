#pragma once
#include "PrimitiveValue.hpp"
class FloatValue:public PrimitiveValue<float>{
    public:
        FloatValue(float value);
        std::wstring toString()override;
};