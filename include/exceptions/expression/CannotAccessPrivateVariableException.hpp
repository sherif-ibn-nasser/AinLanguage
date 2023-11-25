#pragma once
#include "AinException.hpp"
class CannotAccessPrivateVariableException:public AinException{
    public:
        CannotAccessPrivateVariableException(std::wstring trace, std::wstring varName):AinException(
        AinException::errorWString(
                L"في "+trace+L"\n"
                L"لا يُمكن الوصول للمتغير "+varName+L" حيث أنه خاص."
            )
        ){}
};