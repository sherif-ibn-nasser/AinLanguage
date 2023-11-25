#pragma once
#include "IStatement.hpp"
class ContinueStatement:public IStatement{
    public:
        ContinueStatement(
            int lineNumber,
            SharedBaseScope runScope
        );

        void accept(ASTVisitor *visitor) override;
};