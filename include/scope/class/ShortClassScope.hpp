#pragma once
#include "PrimitiveClassScope.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"
class ShortClassScope:public PrimitiveClassScope<int>{
    public:
        ShortClassScope():
        PrimitiveClassScope<int>(Type::SHORT_NAME){}
};