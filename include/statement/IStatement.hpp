#pragma once
#include "ASTNode.hpp"
#include "SharedPtrTypes.hpp"
class IStatement:public ASTNode{
    protected:
        SharedBaseScope runScope;
    public:
        IStatement(int lineNumber,SharedBaseScope runScope);

        SharedBaseScope getRunScope()const;
};