#pragma once
#include "IStatement.hpp"

class ExpressionStatement:public IStatement{
    
    private:
        SharedIExpression ex;
    public:
        ExpressionStatement(
            int lineNumber,
            SharedStmListScope runScope,
            SharedIExpression ex
        );
        void accept(ASTVisitor *visitor) override;

        SharedIExpression getEx()const;
    
};