#pragma once
#include <iostream>
#include "AinException.hpp"

class InvalidEscapeSequenceException:public AinException{

    public:
    InvalidEscapeSequenceException(int lineNumber,std::wstring literal)
    :AinException(
        AinException::errorWString(
            L"في السطر "+std::to_wstring(lineNumber)+
            L" عند "+
            L"\n"+
            AinException::removeNullChar(literal)+
            L"\n"+
            L"يحتوي على حرفٍ خاص غير صالح."
        )
    ){}
};