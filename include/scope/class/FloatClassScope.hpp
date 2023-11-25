#pragma once
#include "PrimitiveClassScope.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"
class FloatClassScope:public PrimitiveClassScope<float>{
    public:
        FloatClassScope():
        PrimitiveClassScope<float>(Type::FLOAT_NAME){}
};