#pragma once
#include "PrimitiveClassScope.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"
class IntClassScope:public PrimitiveClassScope<int>{
    public:
        IntClassScope():
        PrimitiveClassScope<int>(Type::INT_NAME){}
};