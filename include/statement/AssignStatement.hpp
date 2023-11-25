#pragma once
#include "IExpression.hpp"
#include "IStatement.hpp"
#include "SharedPtrTypes.hpp"
#include <memory>

class AssignStatement:public IStatement{
    public:
        AssignStatement(
            int lineNumber,
            SharedStmListScope runScope,
            SharedIExpression left,
            SharedIExpression right
        );

        void accept(ASTVisitor *visitor) override;

        SharedIExpression getLeft()const;

        SharedIExpression getRight()const;
    
    private:
        SharedIExpression left;
        SharedIExpression right;
};