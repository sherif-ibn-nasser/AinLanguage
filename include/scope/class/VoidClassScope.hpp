#pragma once
#include "ClassScope.hpp"
#include "PackageScope.hpp"
#include "Type.hpp"
class VoidClassScope:public ClassScope{
    public:
        VoidClassScope():ClassScope(0,*Type::VOID_NAME,PackageScope::AIN_PACKAGE){}
};