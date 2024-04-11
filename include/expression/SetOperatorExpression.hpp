#pragma once
#include "IExpression.hpp"
#include "SharedPtrTypes.hpp"
class SetOperatorExpression:public IExpression{
    public:
        enum class Operator{
            BAD_OP, // Used as default value
            PLUS_EQUAL,MINUS_EQUAL,
            TIMES_EQUAL,DIV_EQUAL,
            MOD_EQUAL,POW_EQUAL,

            SHR_EQUAL,SHL_EQUAL,
            BIT_AND_EQUAL,XOR_EQUAL,
            BIT_OR_EQUAL,

            PRE_INC,PRE_DEC,
            POST_INC,POST_DEC,

        };

        SetOperatorExpression(
            Operator op,
            SharedOpFunInvokeExpression exOfGet,
            SharedIExpression valueEx
        );

        void accept(ASTVisitor *visitor) override;

        SharedIExpression getExHasGetOp()const;

        SharedOpFunInvokeExpression getExOfGet()const;

        SharedIExpression getIndexEx()const;

        SharedIExpression getValueEx()const;

        SharedFunScope getFunOfSet()const;

        void setFunOfSet(SharedFunScope fun);

        SharedFunScope getFunOfOp()const;

        void setFunOfOp(SharedFunScope fun);

        Operator getOp()const;

        void setOpFunExplicit(bool isOpFunExplicit);
        
        bool isOpFunExplicit();

    private:
        SharedOpFunInvokeExpression exOfGet;
        SharedIExpression valueEx;
        SharedFunScope funOfSet;
        SharedFunScope funOfOp;
        Operator op;
        bool explicitOpFun=false; // Indicates if the augmented assignment operator method is defined in the return type of get ex
};