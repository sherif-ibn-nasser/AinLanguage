#pragma once
#include "AinException.hpp"
class CannotAccessPrivateFunctionException:public AinException{
    public:
        CannotAccessPrivateFunctionException(std::wstring trace, std::wstring decl):AinException(
        AinException::errorWString(
                L"في "+trace+L"\n"
                L"لا يُمكن الوصول لدالة "+decl+L" حيث أنها خاصة."
            )
        ){}
};