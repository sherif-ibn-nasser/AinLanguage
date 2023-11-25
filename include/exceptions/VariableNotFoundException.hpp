#pragma once
#include <iostream>
#include <string>
#include "AinException.hpp"

class VariableNotFoundException:public AinException{

    public:
    VariableNotFoundException(std::wstring trace, std::wstring varName)
    :AinException(
        AinException::errorWString(
            L"في "+trace+L"\n"
            L"لم يتم العثور على المتغير "+varName+L"."
        )
    ){}
};