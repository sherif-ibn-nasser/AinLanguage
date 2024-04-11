#pragma once
#include "ByteClassScope.hpp"
#include "PrimitiveValue.hpp"
#include <memory>
class ByteValue:public PrimitiveValue<int>{
    public:
        ByteValue(int value);
        std::wstring toString()override;
};