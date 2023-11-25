#pragma once
#include "IntClassScope.hpp"
#include "PrimitiveValue.hpp"
#include <memory>
class IntValue:public PrimitiveValue<int>{
    public:
        IntValue(int value);
        std::wstring toString()override;
};