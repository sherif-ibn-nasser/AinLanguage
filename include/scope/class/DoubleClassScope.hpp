#pragma once
#include "PrimitiveClassScope.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"
class DoubleClassScope:public PrimitiveClassScope<long double>{
    public:
        DoubleClassScope():
        PrimitiveClassScope<long double>(Type::DOUBLE_NAME){}
};