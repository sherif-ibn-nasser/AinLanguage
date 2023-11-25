#pragma once
#include "StmListScope.hpp"
class LoopScope:public StmListScope{
    public:
        LoopScope(int lineNumber,SharedBaseScope parentScope);
        void accept(ASTVisitor *visitor) override;
};