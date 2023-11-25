#pragma once
#include "AinException.hpp"
#include <string>
class ArrayNegativeCapacityException:public AinException{
    public:
        ArrayNegativeCapacityException(int cap):
        AinException(
            AinException::errorWString(
                L"سعة المصفوفة سالبة: "+std::to_wstring(cap)
            )
        ){}
};