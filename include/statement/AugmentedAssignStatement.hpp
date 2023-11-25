#pragma once
#include "AssignStatement.hpp"
#include "SharedPtrTypes.hpp"

class AugmentedAssignStatement:public AssignStatement{
    public:
        enum class Operator{
            BAD_OP, // Used as default value
            PLUS,MINUS,TIMES,DIV,MOD,POW,
            SHR,SHL,
            BIT_AND,XOR,BIT_OR
        };
        AugmentedAssignStatement(
            int lineNumber,
            SharedStmListScope runScope,
            Operator op,
            SharedIExpression ex,
            SharedIExpression newValEx
        );

        void accept(ASTVisitor *visitor) override;

        Operator getOp()const;

        SharedFunScope getOpFun()const;

        void setOpFun(SharedFunScope opFun);

    private:
        Operator op;
        SharedFunScope opFun;
};