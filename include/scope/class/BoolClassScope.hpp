#pragma once
#include "PrimitiveClassScope.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"
class BoolClassScope:public PrimitiveClassScope<bool>{
    public:
        BoolClassScope():
        PrimitiveClassScope<bool>(Type::BOOL_NAME){}
};