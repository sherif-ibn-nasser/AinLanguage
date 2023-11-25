#pragma once
#include "SharedPtrTypes.hpp"
struct FunParam{
    SharedWString name;
    SharedType type;

    FunParam(SharedWString name, SharedType type);

    bool operator==(const FunParam& param)const;
    bool operator!=(const FunParam& param)const;
};