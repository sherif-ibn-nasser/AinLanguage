#pragma once
#include "IExpression.hpp"
#include "SharedPtrTypes.hpp"
#include <string>

class VarAccessExpression:public IExpression{
    private:
        std::wstring varName;
        SharedVariable var;
    public:
        VarAccessExpression(int lineNumber,std::wstring varName,SharedVariable var);
        void accept(ASTVisitor *visitor) override;
        SharedVariable getVar() const;
        std::wstring getVarName() const;
        void setVar(SharedVariable var);
};