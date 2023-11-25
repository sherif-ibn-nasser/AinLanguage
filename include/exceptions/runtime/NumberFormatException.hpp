#pragma once
#include "AinException.hpp"
#include <string>
class NumberFormatException:public AinException{
    public:
        NumberFormatException(std::wstring numStr):
        AinException(
            AinException::errorWString(
                L"خطأ في بنية النص \""+numStr+L"\" عند تحويله لعدد."
            )
        ){}
};