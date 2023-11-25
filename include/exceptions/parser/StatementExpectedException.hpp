#pragma once
#include "AinException.hpp"
#include <string>
class StatementExpectedException:public AinException{
    public:
        StatementExpectedException(int lineNumber):AinException(
            AinException::errorWString(
                L"في السطر "+std::to_wstring(lineNumber)+
                L"\nيُتوقَّع أمر برمجي واحد على الأقل."
            )
        ){}
};