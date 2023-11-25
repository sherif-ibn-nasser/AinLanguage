#pragma once
#include <iostream>
#include "AinException.hpp"

class UnclosedCommentException:public AinException{

    public:
    UnclosedCommentException(std::wstring path)
    :AinException(
        AinException::errorWString(
            L"في الملف "+path+
            L"\nلم يتم إغلاق التعليق."
        )
    ){}
};