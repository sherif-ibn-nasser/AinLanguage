#pragma once
#include "SharedPtrTypes.hpp"
#include "StmListScope.hpp"
#include <map>
#include <string>

class FunScope:public StmListScope{
    protected:
        SharedFunDecl decl;
        SharedIValue returnValue;
    public:
        FunScope(
            int lineNumber,
            SharedBaseScope parentScope,
            SharedFunDecl decl
        );
        void accept(ASTVisitor *visitor) override;
        SharedType getReturnType();
        SharedFunDecl getDecl();
        void setReturnValue(SharedIValue returnValue);
        SharedIValue getReturnValue();
};