#pragma once
#include "AinException.hpp"
class OutOfMemoryException:public AinException{
    public:
        OutOfMemoryException():
            AinException(
                AinException::errorWString(
                    L"لا توجد مساحة للتخزين"
                )
            ){}
};