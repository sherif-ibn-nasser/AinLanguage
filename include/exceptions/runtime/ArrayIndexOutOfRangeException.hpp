#pragma once
#include "AinException.hpp"
#include <string>
class ArrayIndexOutOfRangeException:public AinException{
    public:
        ArrayIndexOutOfRangeException(int cap,int index):
        AinException(
            AinException::errorWString(
                L"رقم العنصر: "+std::to_wstring(index)+
                L" خارج نطاق سعة المصفوفة: "+std::to_wstring(cap)
            )
        ){}
};