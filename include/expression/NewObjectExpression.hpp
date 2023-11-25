#pragma once
#include "IExpression.hpp"
#include "SharedPtrTypes.hpp"
#include <string>
class NewObjectExpression:public IExpression{
    private:
        SharedVector<SharedIExpression> args;
        SharedFunScope constructor;
    public:
        NewObjectExpression(
            int lineNumber,
            SharedType type,
            SharedVector<SharedIExpression> args
        );
        void accept(ASTVisitor *visitor) override;
        SharedVector<SharedIExpression> getArgs() const;
        SharedFunScope getConstructor()const;
        void setConstructor(SharedFunScope constructor);
};