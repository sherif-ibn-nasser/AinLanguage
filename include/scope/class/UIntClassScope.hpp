#pragma once
#include "PrimitiveClassScope.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"
class UIntClassScope:public PrimitiveClassScope<unsigned int>{
    public:
        UIntClassScope():
        PrimitiveClassScope<unsigned int>(Type::UINT_NAME){}
};