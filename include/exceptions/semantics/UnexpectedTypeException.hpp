#pragma once
#include "AinException.hpp"
#include <string>
class UnexpectedTypeException:public AinException{
    public:
        UnexpectedTypeException(
            int lineNumber,
            std::wstring expectedTypeName,
            std::wstring foundTypeName
        ):AinException(
            AinException::errorWString(
                L"في السطر "+std::to_wstring(lineNumber)+L": النوع غير صالح\nيُتوقَّع "+
                AinException::betweenAngleBrackets(expectedTypeName)+L" ولكن تم العثور على "+
                AinException::betweenAngleBrackets(foundTypeName)+L"."
            )
        ){}
};