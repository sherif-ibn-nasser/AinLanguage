#pragma once
#include "IStatement.hpp"
#include "SharedPtrTypes.hpp"

class WhileStatement:public IStatement
{
    protected:
        SharedIExpression condition;
        SharedLoopScope loopScope;

    public:
        WhileStatement(
            int lineNumber,
            SharedStmListScope runScope,
            SharedIExpression condition,
            SharedLoopScope loopScope
        );

        void accept(ASTVisitor *visitor) override;

        SharedIExpression getCondition()const;

        SharedLoopScope getLoopScope()const;
};