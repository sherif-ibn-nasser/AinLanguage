#pragma once
#include <iostream>
#include "AinException.hpp"

class OutOfRangeException:public AinException{

    public:
    OutOfRangeException(int lineNumber,std::wstring val)
    :AinException(
        AinException::errorWString(
            L"في السطر "+std::to_wstring(lineNumber)+
            L" عند "+
            L"\n"+
            AinException::removeNullChar(val)+
            L"\n"+
            L"القيمة خارج النطاق المسموح به."
        )
    ){}
};