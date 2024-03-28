#pragma once
#include "ClassScope.hpp"
#include "PackageScope.hpp"
#include "Type.hpp"
#include <string>
class StringClassScope:public ClassScope{
    public:
        StringClassScope();

        static inline auto SIZE_NAME=std::make_shared<std::wstring>(L"الحجم");
};