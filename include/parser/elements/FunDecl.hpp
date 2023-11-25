#pragma once
#include "SharedPtrTypes.hpp"
#include <string>
#include <vector>
struct FunDecl{
    SharedWString name;
    SharedType returnType;
    SharedBool isOperator;
    SharedVector<SharedFunParam> params;
    FunDecl(
        SharedWString name,
        SharedType returnType,
        SharedBool isOperator,
        SharedVector<SharedFunParam> params
    );

    /**
     * @warning This method doesn't check for property [isOperator]
     * and it should be checked explicitly
    */
    bool operator==(const FunDecl& declaration)const;
    
    /**
     * @warning This method doesn't check for property [isOperator]
     * and it should be checked explicitly
    */
    bool operator!=(const FunDecl& declaration)const;

    bool hasImplicitReturnType()const;

    bool isConstructor();

    std::wstring toString();
};