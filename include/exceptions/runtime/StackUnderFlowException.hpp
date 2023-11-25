#pragma once
#include "AinException.hpp"
#include <string>
class StackUnderFlowException:public AinException{
    public:
        StackUnderFlowException():
        AinException(
            AinException::errorWString(
                L"امتلاء في ذاكرة الوصول"
            )
        ){}
};