#pragma once
#include "AinException.hpp"
#include <string>
class UnexpectedTokenException:public AinException{
    public:
    int lineNumber;
    UnexpectedTokenException(
        int lineNumber,
        std::wstring expected,
        std::wstring found
    ):AinException(
        AinException::errorWString(
            L"رمز غير مُتوقَّع\nيُتوقَّع "+
            AinException::betweenAngleBrackets(expected)
            +L"، ولكن وُجِدَ "+
            AinException::betweenAngleBrackets(found)
        )
    ),lineNumber(lineNumber){}
};