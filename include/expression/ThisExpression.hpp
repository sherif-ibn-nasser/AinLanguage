#pragma once
#include "IExpression.hpp"
#include "SharedPtrTypes.hpp"
#include <string>
class ThisExpression:public IExpression{
    public:
        ThisExpression(int lineNumber,SharedClassScope classScope);
        void accept(ASTVisitor *visitor) override;
};