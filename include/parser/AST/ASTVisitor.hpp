#pragma once

class PackageScope;
class FileScope;
class ClassScope;
class FunScope;
class BuiltInFunScope;
class LoopScope;
class StmListScope;

class VarStm;
class IfStatement;
class AssignStatement;
class AugmentedAssignStatement;
class WhileStatement;
class DoWhileStatement;
class BreakStatement;
class ContinueStatement;
class ReturnStatement;
class ExpressionStatement;

class VarAccessExpression;
class FunInvokeExpression;
class NewObjectExpression;
class NewArrayExpression;
class LiteralExpression;
class UnitExpression;

class LogicalExpression;
class NonStaticVarAccessExpression;
class NonStaticFunInvokeExpression;
class OperatorFunInvokeExpression;
class SetOperatorExpression;

class ThisExpression;
class ThisVarAccessExpression;
class ThisFunInvokeExpression;

class ASTVisitor{
    public:
        virtual void visit(PackageScope* scope);
        virtual void visit(FileScope* scope);
        virtual void visit(ClassScope* scope);
        virtual void visit(FunScope* scope);
        virtual void visit(BuiltInFunScope* scope);
        virtual void visit(LoopScope* scope);
        virtual void visit(StmListScope* scope);

        virtual void visit(VarStm* stm);
        virtual void visit(AssignStatement* stm);
        virtual void visit(AugmentedAssignStatement* stm);
        virtual void visit(IfStatement* stm);
        virtual void visit(WhileStatement* stm);
        virtual void visit(DoWhileStatement* stm);
        virtual void visit(BreakStatement* stm);
        virtual void visit(ContinueStatement* stm);
        virtual void visit(ReturnStatement* stm);
        virtual void visit(ExpressionStatement* stm);

        virtual void visit(VarAccessExpression* ex);
        virtual void visit(FunInvokeExpression* ex);
        virtual void visit(NewObjectExpression* ex);
        virtual void visit(NewArrayExpression* ex);
        virtual void visit(LiteralExpression* ex);
        virtual void visit(UnitExpression* ex);

        virtual void visit(LogicalExpression* ex);
        virtual void visit(NonStaticVarAccessExpression* ex);
        virtual void visit(NonStaticFunInvokeExpression* ex);
        virtual void visit(OperatorFunInvokeExpression* ex);
        virtual void visit(SetOperatorExpression* ex);

        virtual void visit(ThisExpression* ex);
        virtual void visit(ThisVarAccessExpression* ex);
        virtual void visit(ThisFunInvokeExpression* ex);

        virtual ~ASTVisitor();

};