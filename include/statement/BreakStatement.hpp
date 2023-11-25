#pragma once
#include "IStatement.hpp"
#include "SharedPtrTypes.hpp"
class BreakStatement:public IStatement{
    public:
        BreakStatement(
            int lineNumber,
            SharedBaseScope runScope
        );

        void accept(ASTVisitor *visitor) override;
};