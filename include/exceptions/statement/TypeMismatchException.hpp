#pragma once
#include "AinException.hpp"
#include <string>
class TypeMismatchException:public AinException{
    public:
        TypeMismatchException(
            std::wstring first,
            std::wstring second
        ):AinException(
            AinException::errorWString(
                L"يجب أن يكونا من نفس النوع "+
                AinException::betweenAngleBrackets(first)+L" و"+
                AinException::betweenAngleBrackets(second)+L" ."
            )
        ){}
};