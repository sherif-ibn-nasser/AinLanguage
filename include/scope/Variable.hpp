#pragma once
#include "SharedPtrTypes.hpp"
class Variable{
    private:
        SharedVarDecl decl;
    public:
        Variable(SharedWString name,SharedType type,SharedBool isVal);
        Variable(SharedVarDecl decl);

        SharedWString getName();

        SharedBool isValue();

        bool hasImplicitType();

        SharedType getType();

        void setType(SharedType type);

        int getSize();

};