#pragma once
#include "ClassScope.hpp"
#include "PackageScope.hpp"
#include "Type.hpp"
class UnitClassScope:public ClassScope{
    public:
        UnitClassScope():ClassScope(0,*Type::UNIT_NAME,PackageScope::AIN_PACKAGE){}
};