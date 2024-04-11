#pragma once
#include "PrimitiveClassScope.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"
class ByteClassScope:public PrimitiveClassScope<int>{
    public:
        ByteClassScope():
        PrimitiveClassScope<int>(Type::BYTE_NAME){}
};