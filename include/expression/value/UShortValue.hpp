#pragma once
#include "ByteClassScope.hpp"
#include "PrimitiveValue.hpp"
#include <memory>
class UShortValue:public PrimitiveValue<unsigned int>{
    public:
        UShortValue(unsigned int value);
        std::wstring toString()override;
};