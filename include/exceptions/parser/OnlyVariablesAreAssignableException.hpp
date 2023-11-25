#pragma once
#include "AinException.hpp"
class OnlyVariablesAreAssignableException:public AinException{
    public:
    int lineNumber;
    OnlyVariablesAreAssignableException(
        int lineNumber
    ):AinException(
        AinException::errorWString(
            L"فقط المتغيرات التي تقبل تخصيص قيمتها."
        )
    ),lineNumber(lineNumber){}
};