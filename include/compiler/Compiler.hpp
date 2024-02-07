#pragma once

#include <string>
#include <unordered_map>
#include "ASTVisitor.hpp"
#include "CompilerVarsOffsetSetter.hpp"

#include "PackageScope.hpp"
#include "FileScope.hpp"
#include "ClassScope.hpp"
#include "FunScope.hpp"
#include "LoopScope.hpp"
#include "SharedPtrTypes.hpp"
#include "StmListScope.hpp"
#include "VarStm.hpp"
#include "IfStatement.hpp"
#include "AssignStatement.hpp"
#include "AugmentedAssignStatement.hpp"
#include "Variable.hpp"
#include "WhileStatement.hpp"
#include "DoWhileStatement.hpp"
#include "BreakStatement.hpp"
#include "ContinueStatement.hpp"
#include "ReturnStatement.hpp"
#include "ExpressionStatement.hpp"
#include "VarAccessExpression.hpp"
#include "FunInvokeExpression.hpp"
#include "NewObjectExpression.hpp"
#include "NewArrayExpression.hpp"
#include "LiteralExpression.hpp"
#include "UnitExpression.hpp"
#include "LogicalExpression.hpp"
#include "NonStaticVarAccessExpression.hpp"
#include "NonStaticFunInvokeExpression.hpp"
#include "OperatorFunInvokeExpression.hpp"
#include "SetOperatorExpression.hpp"
#include "ThisExpression.hpp"
#include "ThisVarAccessExpression.hpp"
#include "ThisFunInvokeExpression.hpp"

class BuiltInFunScope;

class Compiler:public ASTVisitor{
    private:
        int funLabelsSize=0; // for numbering labels for functions
        int constructorLabelsSize=0; // for numbering labels for constructors
        int methodLabelsSize=0; // for numbering labels for methods in all classes
        std::wstring dataAsm=L"";
        std::wstring bssAsm=L"";
        std::wstring textAsm=
            L"section .text\n"
            L"\tglobal _start\n"
        ;

        std::unordered_map<StmListScope*, std::pair<std::wstring, std::wstring>> labelsAsm; // first of pair is for label name, second for the full label's text

        std::wstring* currentLabelAsm;

        void reserveSpaceOnStack(int size);
        void removeReservedSpaceFromStack(int size);
        void addExit(int errorCode);
        void addExit0();
        int getVariableSize(SharedVariable var);
        int getVariablesSize(SharedMap<std::wstring, SharedVariable> vars);
        std::wstring getAsmValue(SharedIValue value);
        std::wstring getAsmSize(int size);
        std::wstring getRaxBySize(int size);
        std::wstring getAsmLabelName(StmListScope* scope);
        std::wstring* getAsmLabelInstructions(StmListScope* scope);
        
    public:
        void visit(PackageScope* scope)override;
        void visit(FileScope* scope)override;
        void visit(ClassScope* scope)override;
        void visit(FunScope* scope)override;
        void visit(BuiltInFunScope* scope)override;
        void visit(LoopScope* scope)override;
        void visit(StmListScope* scope)override;
        void visit(VarStm* stm)override;
        void visit(AssignStatement* stm)override;
        void visit(AugmentedAssignStatement* stm)override;
        void visit(IfStatement* stm)override;
        void visit(WhileStatement* stm)override;
        void visit(DoWhileStatement* stm)override;
        void visit(BreakStatement* stm)override;
        void visit(ContinueStatement* stm)override;
        void visit(ReturnStatement* stm)override;
        void visit(ExpressionStatement* stm)override;
        void visit(VarAccessExpression* ex)override;
        void visit(FunInvokeExpression* ex)override;
        void visit(NewObjectExpression* ex)override;
        void visit(NewArrayExpression* ex)override;
        void visit(LiteralExpression* ex)override;
        void visit(UnitExpression* ex)override;
        void visit(LogicalExpression* ex)override;
        void visit(NonStaticVarAccessExpression* ex)override;
        void visit(NonStaticFunInvokeExpression* ex)override;
        void visit(OperatorFunInvokeExpression* ex)override;
        void visit(SetOperatorExpression* ex)override;
        void visit(ThisExpression* ex)override;
        void visit(ThisVarAccessExpression* ex)override;
        void visit(ThisFunInvokeExpression* ex)override;

        std::wstring getAssemblyFile();

        std::unordered_map<Variable*, CompilerVarsOffsetSetter::Offset> offsets;

        static const inline auto RAX=L"RAX";
        static const inline auto RBX=L"RBX";
        static const inline auto RCX=L"RCX";
        static const inline auto RDX=L"RDX";
        static const inline auto RDI=L"RDI";
        static const inline auto RSP=L"RSP";
        static const inline auto RBP=L"RBP";
        static const inline auto RDS=L"RDS";
        static const inline auto RSS=L"RSS";
};