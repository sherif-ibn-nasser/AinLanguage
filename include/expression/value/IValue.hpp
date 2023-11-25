#pragma once
#include "SharedPtrTypes.hpp"
#include <string>
class IValue{
    protected:
        SharedType type;
        IValue(SharedType type);
    public:
        SharedType getType();
        virtual ~IValue();
        virtual std::wstring toString()=0;
};