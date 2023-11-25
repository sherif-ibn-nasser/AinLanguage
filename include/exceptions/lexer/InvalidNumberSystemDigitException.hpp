#pragma once
#include <iostream>
#include <algorithm>
#include "AinException.hpp"

class InvalidNumberSystemDigitException:public AinException{

    public:

    InvalidNumberSystemDigitException(
        int lineNumber,
        std::wstring errorNumberToken
    )
    :AinException(
        AinException::errorWString(
            L"في السطر "+std::to_wstring(lineNumber)+
            L" عند "+
            L"\n"+
            AinException::removeNullChar(errorNumberToken)+
            L"\n"+
            L"يحتوى على حرف أو رقم لا ينتمي للنظام العددي "+
            (
                (std::towlower(errorNumberToken[1])==L'b')?L"الثُنائي.":
                (std::towlower(errorNumberToken[1])==L'o')?L"الثماني.":
                (std::towlower(errorNumberToken[1])==L'x')?L"السُداسي عَشري.":
                L"العَشري."
            )
        )
    ){}
};