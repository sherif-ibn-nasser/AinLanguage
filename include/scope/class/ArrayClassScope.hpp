#pragma once
#include "PrimitiveClassScope.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"
#include "Variable.hpp"
#include <memory>
#include <string>
class ArrayClassScope:public PrimitiveClassScope<std::vector<SharedIValue>>{
    public:
        static inline auto CAPACITY_NAME=std::make_shared<std::wstring>(L"السعة");

        static SharedFunScope GET;

        static SharedFunScope SET;

        ArrayClassScope();
};