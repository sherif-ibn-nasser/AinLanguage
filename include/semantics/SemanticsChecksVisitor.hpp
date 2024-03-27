#pragma once
#include "ASTVisitor.hpp"

#include "IStatement.hpp"
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
#include "ThisVarAccessExpression.hpp"
#include "ThisFunInvokeExpression.hpp"
#include <string>
#include <vector>

class SemanticsChecksVisitor:public ASTVisitor{
    public:
        void visit(PackageScope* scope)override;
        void visit(FileScope* scope)override;
        void visit(ClassScope* scope)override;
        void visit(FunScope* scope)override;
        void visit(LoopScope* scope)override;
        void visit(StmListScope* scope)override;

        void visit(VarStm* stm)override;
        void visit(AssignStatement* stm)override;
        void visit(AugmentedAssignStatement* stm)override;
        void visit(IfStatement* stm)override;
        void visit(WhileStatement* stm)override;
        void visit(DoWhileStatement* stm)override;
        void visit(ReturnStatement* stm)override;
        void visit(ExpressionStatement* stm)override;

        void visit(VarAccessExpression* ex)override;
        void visit(FunInvokeExpression* ex)override;
        void visit(NewObjectExpression* ex)override;
        void visit(NewArrayExpression* ex)override;

        void visit(LogicalExpression* ex)override;
        void visit(NonStaticVarAccessExpression* ex)override;
        void visit(NonStaticFunInvokeExpression* ex)override;
        void visit(OperatorFunInvokeExpression* ex)override;
        void visit(SetOperatorExpression* ex)override;

        void visit(ThisVarAccessExpression* ex)override;
        void visit(ThisFunInvokeExpression* ex)override;

    private:
        SharedBaseScope checkScope;
        void initStmRunScope(IStatement* stm);
        void doWhileStmChecks(WhileStatement* stm);
        void doStmListScopeChecks(StmListScope* scope);
        void doOperatorFunChecks(FunScope* scope);
        void checkOperatorFunParamsSize(FunScope* scope);
        void checkOperatorFunReturnType(FunScope* scope);
        std::wstring getAugmentedAssignOpFunName(AugmentedAssignStatement::Operator op);
        std::wstring getExplicitAugmentedAssignOpFunName(AugmentedAssignStatement::Operator op);
        std::wstring getOpFunNameOfSetOp(SetOperatorExpression::Operator op);
        std::wstring getExplicitOpFunNameOfSetOp(SetOperatorExpression::Operator op);
        SharedFunScope findFunInType(
            SharedType type,
            std::wstring funName,
            std::vector<SharedType> paramsTypes,
            int traceLineNumber
        );
        SharedFunScope findOpFunInType(
            SharedType type,
            std::wstring funName,
            std::vector<SharedType> paramsTypes,
            int traceLineNumber
        );
};