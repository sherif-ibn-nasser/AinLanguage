#pragma once
#include <iostream>
#include "AinException.hpp"

class IllegalCommaException:public AinException{

    public:
    IllegalCommaException(int lineNumber,std::wstring token)
    :AinException(
        AinException::errorWString(
            L"في السطر "+std::to_wstring(lineNumber)+
            L" عند "+
            L"\n"+
            AinException::removeNullChar(token)+
            L"\n"+
            L"يوجد فاصلة غير صالحة في الرقم."
        )
    ){}
};