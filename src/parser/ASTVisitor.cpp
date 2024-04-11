#include "ASTVisitor.hpp"

#include "NewArrayExpression.hpp"
#include "PackageScope.hpp"
#include "FileScope.hpp"
#include "ClassScope.hpp"
#include "FunScope.hpp"
#include "BuiltInFunScope.hpp"
#include "LoopScope.hpp"
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
#include "VoidExpression.hpp"
#include "LogicalExpression.hpp"
#include "NonStaticVarAccessExpression.hpp"
#include "NonStaticFunInvokeExpression.hpp"
#include "OperatorFunInvokeExpression.hpp"
#include "SetOperatorExpression.hpp"
#include "ThisExpression.hpp"
#include "ThisVarAccessExpression.hpp"
#include "ThisFunInvokeExpression.hpp"

ASTVisitor::~ASTVisitor(){}

void ASTVisitor::visit(PackageScope* scope){}

void ASTVisitor::visit(FileScope* scope){}

void ASTVisitor::visit(ClassScope* scope){}

void ASTVisitor::visit(FunScope* scope){}

void ASTVisitor::visit(BuiltInFunScope* scope){}

void ASTVisitor::visit(LoopScope* scope){}

void ASTVisitor::visit(StmListScope* scope){}

void ASTVisitor::visit(VarStm* stm){}

void ASTVisitor::visit(AssignStatement* stm){}

void ASTVisitor::visit(AugmentedAssignStatement* stm){}

void ASTVisitor::visit(IfStatement* stm){}

void ASTVisitor::visit(WhileStatement* stm){}

void ASTVisitor::visit(DoWhileStatement* stm){}

void ASTVisitor::visit(BreakStatement* stm){}

void ASTVisitor::visit(ContinueStatement* stm){}

void ASTVisitor::visit(ReturnStatement* stm){}

void ASTVisitor::visit(ExpressionStatement* stm){}

void ASTVisitor::visit(VarAccessExpression* ex){}

void ASTVisitor::visit(FunInvokeExpression* ex){}

void ASTVisitor::visit(NewObjectExpression* ex){}

void ASTVisitor::visit(NewArrayExpression* ex){}

void ASTVisitor::visit(LiteralExpression* ex){}

void ASTVisitor::visit(VoidExpression* ex){}

void ASTVisitor::visit(LogicalExpression* ex){}

void ASTVisitor::visit(NonStaticVarAccessExpression* ex){}

void ASTVisitor::visit(NonStaticFunInvokeExpression* ex){}

void ASTVisitor::visit(OperatorFunInvokeExpression* ex){}

void ASTVisitor::visit(SetOperatorExpression* ex){}

void ASTVisitor::visit(ThisExpression* ex){}

void ASTVisitor::visit(ThisVarAccessExpression* ex){}

void ASTVisitor::visit(ThisFunInvokeExpression* ex){}

void PackageScope::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void FileScope::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void ClassScope::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void FunScope::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void BuiltInFunScope::accept(ASTVisitor *visitor){
    visitor->visit(this);
}

void LoopScope::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void StmListScope::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void VarStm::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void IfStatement::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void AssignStatement::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void AugmentedAssignStatement::accept(ASTVisitor *visitor){
    visitor->visit(this);
}

void WhileStatement::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void DoWhileStatement::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void BreakStatement::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void ContinueStatement::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void ReturnStatement::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void ExpressionStatement::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void VarAccessExpression::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void FunInvokeExpression::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void NewObjectExpression::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void NewArrayExpression::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void LiteralExpression::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void VoidExpression::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void LogicalExpression::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void NonStaticVarAccessExpression::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void NonStaticFunInvokeExpression::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void OperatorFunInvokeExpression::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void SetOperatorExpression::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void ThisFunInvokeExpression::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void ThisVarAccessExpression::accept(ASTVisitor* visitor){
    visitor->visit(this);
}

void ThisExpression::accept(ASTVisitor* visitor){
    visitor->visit(this);
}