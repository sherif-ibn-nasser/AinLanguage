#pragma once
#include "SharedPtrTypes.hpp"
struct VarDecl{
    SharedWString name;
    SharedType type;
    SharedBool isVal;

    VarDecl(
        SharedWString name,
        SharedType type,
        SharedBool isVal
    );

    bool operator==(const VarDecl& declaration)const;
    
    bool operator!=(const VarDecl& declaration)const;

    bool hasImplicitType()const;
};