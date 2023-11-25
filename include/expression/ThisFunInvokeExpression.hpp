#pragma once
#include "IExpression.hpp"
#include "SharedPtrTypes.hpp"
#include <string>
class ThisFunInvokeExpression:public IExpression{
    protected:
        std::wstring funName;
        SharedFunScope fun;
        SharedVector<SharedIExpression> args;
        SharedClassScope classScope;
    public:
        ThisFunInvokeExpression(
            int lineNumber,
            SharedClassScope classScope,
            std::wstring funName,
            SharedVector<SharedIExpression> args
        );
        void accept(ASTVisitor *visitor) override;
        
        virtual ~ThisFunInvokeExpression();

        std::wstring getFunName() const;

        SharedClassScope getClassScope() const;

        SharedVector<SharedIExpression> getArgs() const;

        SharedFunScope getFun()const;

        void setFun(SharedFunScope fun);
};