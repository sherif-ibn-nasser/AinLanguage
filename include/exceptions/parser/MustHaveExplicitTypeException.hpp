#pragma once
#include "AinException.hpp"
class MustHaveExplicitTypeException:public AinException{
    public:
    int lineNumber;
    MustHaveExplicitTypeException(
        int lineNumber
    ):AinException(
        AinException::errorWString(
            L"يجب تحديد النوع."
        )
    ),lineNumber(lineNumber){}
};