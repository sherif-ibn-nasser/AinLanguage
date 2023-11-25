#pragma once
#include "PrimitiveClassScope.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"
class ULongClassScope:public PrimitiveClassScope<unsigned long long>{
    public:
        ULongClassScope():
        PrimitiveClassScope<unsigned long long>(Type::ULONG_NAME){}
};