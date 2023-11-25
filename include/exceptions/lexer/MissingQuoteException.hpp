#pragma once
#include <iostream>
#include "AinException.hpp"

class MissingQuoteException:public AinException{

    public:
    MissingQuoteException(int lineNumber,std::wstring literal)
    :AinException(
        AinException::errorWString(
            L"في السطر "+std::to_wstring(lineNumber)+
            L" عند "+
            L"\n"+
            AinException::removeNullChar(literal)+
            L"\n"
            +L"توجد علامة تنصيص مفقودة."
        )
    ){}
};