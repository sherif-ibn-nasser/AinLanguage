#pragma once
#include "ASTNode.hpp"
#include "SharedPtrTypes.hpp"
#include <memory>
#include <string>
class IExpression:public ASTNode{
    protected:
        int lineNumber;
        SharedType returnType;
    public:
        IExpression(int lineNumber,SharedType returnType);
        SharedType getReturnType();
        void setReturnType(SharedType returnType);
        virtual ~IExpression();
        static bool isAssignableExpression(SharedIExpression ex);
        static SharedOpFunInvokeExpression isGetOpFunInvokeExpression(SharedIExpression ex);
};