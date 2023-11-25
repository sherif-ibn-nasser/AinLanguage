#pragma once
#include "AinException.hpp"
#include <string>
class ExpressionExpectedException:public AinException{
    public:
        ExpressionExpectedException(int lineNumber):AinException(
            AinException::errorWString(
                L"في السطر "+std::to_wstring(lineNumber)+
                L"\nيُتوقَّع تعبير برمجي."
            )
        ){}
};