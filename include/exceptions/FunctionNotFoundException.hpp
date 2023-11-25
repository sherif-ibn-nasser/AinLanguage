#pragma once
#include <iostream>
#include <string>
#include "AinException.hpp"

class FunctionNotFoundException:public AinException{

    public:
    FunctionNotFoundException(std::wstring trace, std::wstring decl)
    :AinException(
        AinException::errorWString(
            L"في "+trace+L"\n"
            L"لم يتم العثور على دالة "+decl+L"."
        )
    ){}
};