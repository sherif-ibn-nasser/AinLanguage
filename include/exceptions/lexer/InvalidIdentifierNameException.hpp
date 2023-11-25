#pragma once
#include <iostream>
#include "AinException.hpp"

class InvalidIdentifierNameException:public AinException{

    public:
    InvalidIdentifierNameException(int lineNumber,std::wstring token)
    :AinException(
        AinException::errorWString(
            L"في السطر "+std::to_wstring(lineNumber)+
            L"\n"+
            L"المُعرف "+
            AinException::removeNullChar(token)+
            L"\n"+
            L"يجب ألا يبدأ بأرقام، وألا يكون كلمةً محجوزة."
        )
    ){}
};