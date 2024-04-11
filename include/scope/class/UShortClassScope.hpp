#pragma once
#include "PrimitiveClassScope.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"
class UShortClassScope:public PrimitiveClassScope<unsigned int>{
    public:
        UShortClassScope():
        PrimitiveClassScope<unsigned int>(Type::USHORT_NAME){}
};