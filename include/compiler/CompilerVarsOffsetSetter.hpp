#pragma once
#include "PackageScope.hpp"
#include "FileScope.hpp"
#include "ClassScope.hpp"
#include "FunScope.hpp"
#include "StmListScope.hpp"
#include "IfStatement.hpp"
#include "WhileStatement.hpp"
#include "DoWhileStatement.hpp"
#include <string>
class CompilerVarsOffsetSetter:public ASTVisitor{
    public:
        int stmListScopeOffset;
        int globalVarsOffset=0; // 1st address in data segment
        struct Offset{
            Offset(std::string reg, int value);
            Offset();
            std::string reg;
            int value=0;
        };
        std::unordered_map<Variable*, Offset>* offsets;
        CompilerVarsOffsetSetter(
            std::unordered_map<Variable*, Offset>* offsets
        );
    private:
        void offsetStmListScope(StmListScope* scope);
        void visit(PackageScope* scope)override;
        void visit(FileScope* scope)override;
        void visit(ClassScope* scope)override;
        void visit(FunScope* scope)override;
        void visit(IfStatement* stm)override;
        void visit(WhileStatement* stm)override;
        void visit(DoWhileStatement* stm)override;
};