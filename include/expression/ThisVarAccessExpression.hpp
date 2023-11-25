#pragma once
#include "IExpression.hpp"
#include "SharedPtrTypes.hpp"
#include <string>
class ThisVarAccessExpression:public IExpression{
    private:
        std::wstring varName;
        SharedClassScope classScope;
        SharedVariable var;
    public:
        ThisVarAccessExpression(
            int lineNumber,
            SharedClassScope classScope,
            std::wstring varName
        );
        void accept(ASTVisitor *visitor) override;

        std::wstring getVarName() const;

        SharedClassScope getClassScope() const;

        SharedVariable getVar()const;

        void setVar(SharedVariable var);
};