#pragma once
#include <iostream>
#include "AinException.hpp"

class UnsupportedTokenException:public AinException{

    public:
    UnsupportedTokenException(int lineNumber,std::wstring token)
    :AinException(
        AinException::errorWString(
            L"في السطر "+std::to_wstring(lineNumber)+
            L" عند "+
            L"\n"+
            AinException::removeNullChar(token)+
            L"\n"+
            L"قيمة غير مدعومة."
        )
    ){}
};