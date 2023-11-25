#pragma once
#include "AinException.hpp"
#include <string>
class StackOverFlowException:public AinException{
    public:
        StackOverFlowException():
        AinException(
            AinException::errorWString(
                L"امتلاء في ذاكرة الوصول"
            )
        ){}
};