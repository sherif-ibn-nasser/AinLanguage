#pragma once
#include "AinException.hpp"
#include <string>
class TokenIsNotAllowedHereException:public AinException{
    public:
    int lineNumber;
    TokenIsNotAllowedHereException(
        int lineNumber,
        std::wstring token
    ):AinException(
        AinException::errorWString(
            L"الرمز "+
            AinException::betweenAngleBrackets(token)
            +L" غير مسموح به هنا"
        )
    ),lineNumber(lineNumber){}
};