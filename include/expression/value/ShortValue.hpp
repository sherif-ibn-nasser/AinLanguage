#pragma once
#include "ByteClassScope.hpp"
#include "PrimitiveValue.hpp"
#include <memory>
class ShortValue:public PrimitiveValue<int>{
    public:
        ShortValue(int value);
        std::wstring toString()override;
};