#pragma once
#include "ClassScope.hpp"
#include "PackageScope.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"
template<typename T>
class PrimitiveClassScope:public ClassScope{
    public:
        PrimitiveClassScope(SharedWString name);
};

template<typename T>
PrimitiveClassScope<T>::PrimitiveClassScope(SharedWString name)
:ClassScope(0,*name,PackageScope::AIN_PACKAGE)
{}