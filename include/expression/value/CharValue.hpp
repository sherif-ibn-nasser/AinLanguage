#pragma once
#include "PrimitiveValue.hpp"
class CharValue:public PrimitiveValue<wchar_t>{
    public:
        CharValue(wchar_t value);
        std::wstring toString()override;
};