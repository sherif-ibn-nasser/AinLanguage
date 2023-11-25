#pragma once
#include "PrimitiveClassScope.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"
class LongClassScope:public PrimitiveClassScope<long long>{
    public:
        LongClassScope():
        PrimitiveClassScope<long long>(Type::LONG_NAME){}
};