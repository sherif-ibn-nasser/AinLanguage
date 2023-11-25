#pragma once
#include "PrimitiveClassScope.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"
class CharClassScope:public PrimitiveClassScope<wchar_t>{
    public:
        CharClassScope():
        PrimitiveClassScope<wchar_t>(Type::CHAR_NAME){}
};