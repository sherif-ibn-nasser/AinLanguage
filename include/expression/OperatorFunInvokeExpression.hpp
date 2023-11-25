#pragma once
#include "NonStaticFunInvokeExpression.hpp"
#include "SharedPtrTypes.hpp"
#include <string>
class OperatorFunInvokeExpression:public NonStaticFunInvokeExpression{
    public:
        enum class Operator{
            PLUS,MINUS,TIMES,DIV,MOD,POW,
            EQUAL_EQUAL,NOT_EQUAL,
            LESS,LESS_EQUAL,GREATER,GREATER_EQUAL,
            SHR,SHL,BIT_AND,XOR,BIT_OR,
            UNARY_PLUS,UNARY_MINUS,
            LOGICAL_NOT,BIT_NOT,
            PRE_INC,PRE_DEC,POST_INC,POST_DEC,
            GET,
            SET_EQUAL, // Should be used when the operator of set is equal
        };
        OperatorFunInvokeExpression(
            int lineNumber,
            Operator op,
            SharedVector<SharedIExpression> args,
            SharedIExpression inside
        );
        
        void accept(ASTVisitor *visitor) override;

        Operator getOp()const;

    private:
        Operator op;
};