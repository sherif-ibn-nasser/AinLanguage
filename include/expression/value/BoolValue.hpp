#pragma once
#include "PrimitiveValue.hpp"
class BoolValue:public PrimitiveValue<bool>{
    public:
        BoolValue(bool value);
        std::wstring toString()override;
};