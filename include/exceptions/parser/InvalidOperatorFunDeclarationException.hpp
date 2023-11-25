#pragma once
#include "AinException.hpp"
#include <string>
class InvalidOperatorFunDeclarationException:public AinException{
    public:
    InvalidOperatorFunDeclarationException(
        std::wstring reason
    ):AinException(
        AinException::errorWString(
            L"دالة مؤثر غير صالحة: "+reason
        )
    ){}
};