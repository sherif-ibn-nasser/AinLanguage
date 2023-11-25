#pragma once
#include "AinException.hpp"
#include <string>
class InvalidConstructorDeclarationException:public AinException{
    public:
    InvalidConstructorDeclarationException(
        std::wstring reason
    ):AinException(
        AinException::errorWString(
            L"دالة إنشاء غير صالحة: "+reason
        )
    ){}
};