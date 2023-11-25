#pragma once
#include <iostream>
#include "AinException.hpp"

class InvalidLengthCharacterLiteralException:public AinException{

    public:
    InvalidLengthCharacterLiteralException(int lineNumber,std::wstring literal)
    :AinException(
        AinException::errorWString(
            L"في السطر "+std::to_wstring(lineNumber)+
            L" عند "+
            L"\n"+
            AinException::removeNullChar(literal)+
            L"\n"+
            L"يجب أن يحتوى على حرفٍ واحدٍ فقط."
        )
    ){}
};