#pragma once
#include "PrimitiveClassScope.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"
class UByteClassScope:public PrimitiveClassScope<unsigned int>{
    public:
        UByteClassScope():
        PrimitiveClassScope<unsigned int>(Type::UBYTE_NAME){}
};