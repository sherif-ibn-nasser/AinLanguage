#pragma once
#include "SharedPtrTypes.hpp"
#include "WhileStatement.hpp"

class DoWhileStatement:public WhileStatement
{
    public:
        using WhileStatement::WhileStatement;
        void accept(ASTVisitor *visitor) override;
};