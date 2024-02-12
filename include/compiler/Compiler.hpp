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
#include "Assembler.hpp"

class BuiltInFunScope;

class Compiler:public ASTVisitor{
    private:
        int funLabelsSize=0; // for numbering labels for functions
        int constructorLabelsSize=0; // for numbering labels for constructors
        int methodLabelsSize=0; // for numbering labels for methods in all classes
        int currentLoopLabelsSize=0; // for numbering labels for loops in a function
        int currentIfLabelsSize=0; // for numbering labels for if statements in a function
        int currentLogicalShortcutsLabelsSize=0; // for numbering labels for if statements in a function
        std::wstring dataAsm=
            L"section .data\n"
            L"\tbrk_end dq 0\n";
        std::wstring bssAsm=L"";
        std::wstring textAsm=
            L"section .text\n"
            L"\tglobal _start"  // new line will be added in getAssemblyFile()
        ;

        std::unordered_map<StmListScope*, Assembler::AsmLabel> labelsAsm; // first of pair is for label name, second for the full label's text

        std::unordered_map<Variable*, void*> inUseGlobalVariables; // The global variable that are accessed in the user code, we count them to optimize the asm

        Assembler::AsmLabel* currentAsmLabel;
        Assembler::AsmLabel* startAsmLabel;
        Assembler::AsmLabel* initAsmLabel;
        
        int getVariableSize(Variable* var);
        int getVariablesSize(SharedMap<std::wstring, SharedVariable> vars);
        void optimizeConditionalJumpInstruction(IExpression* condition, Assembler::AsmOperand label, std::wstring comment=L"");
        void optimizeNegatedConditionalJumpInstruction(IExpression* condition, Assembler::AsmOperand label, std::wstring comment=L"");
        void visitLoopStm(WhileStatement *stm, bool isDoWhileStm=false); // DoWhileStatement is a child of WhileStatement
        // Will return the size of params+non_params+locals in nested scopes if scope is a function scope
        int getLocalsSize(StmListScope *scope);

        void invokeBuiltInOpFun(OperatorFunInvokeExpression* ex);
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

};