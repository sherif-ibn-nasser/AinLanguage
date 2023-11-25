#pragma once
#include "AinException.hpp"
#include "KeywordToken.hpp"
#include <string>
class StmShouldBeUsedInsideLoopException:public AinException{
    public:
        StmShouldBeUsedInsideLoopException(
            int lineNumber,
            std::wstring stm
        ):AinException(
            AinException::errorWString(
                stm+L" يجب أن تُستعمل من داخل حلقة تكرارية."
            )
        ){}
};