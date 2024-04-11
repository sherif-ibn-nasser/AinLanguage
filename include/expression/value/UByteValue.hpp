#pragma once
#include "UByteClassScope.hpp"
#include "PrimitiveValue.hpp"
#include <memory>
class UByteValue:public PrimitiveValue<unsigned int>{
    public:
        UByteValue(unsigned int value);
        std::wstring toString()override;
};