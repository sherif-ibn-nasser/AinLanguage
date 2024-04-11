#pragma once
#include "IValue.hpp"
#include "Type.hpp"
class VoidValue:public IValue{
    public:
        VoidValue();
        std::wstring toString() override;
};