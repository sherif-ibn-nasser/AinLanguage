#pragma once
#include "IExpression.hpp"
#include "Type.hpp"
class VoidExpression:public IExpression{
    public:
        VoidExpression(int lineNumber);
        void accept(ASTVisitor *visitor) override;
};