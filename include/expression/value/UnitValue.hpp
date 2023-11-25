#pragma once
#include "IValue.hpp"
#include "Type.hpp"
class UnitValue:public IValue{
    public:
        UnitValue();
        std::wstring toString() override;
};