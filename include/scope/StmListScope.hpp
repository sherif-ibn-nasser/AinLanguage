#pragma once
#include "BaseScope.hpp"
#include "SharedPtrTypes.hpp"
#include <string>

class StmListScope:public BaseScope{

    protected:
        SharedMap<std::wstring, SharedVariable> locals;
        SharedStmList stmList;

    public:
        StmListScope(int lineNumber,std::wstring name,SharedBaseScope parentScope);
        void accept(ASTVisitor *visitor) override;
        SharedMap<std::wstring, SharedVariable> getLocals();
        SharedStmList getStmList();
        SharedVariable getVarByName(std::wstring varName)override;
        SharedVariable getLocalByName(std::wstring varName);
};