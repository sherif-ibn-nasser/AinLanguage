#pragma once
#include "ASTVisitor.hpp"

#include "IValue.hpp"
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
#include "VarsOffsetSetter.hpp"
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

#include "RefValue.hpp"
#include <memory>
#include <unordered_map>

#define DATA_SIZE  64
#define HEAP_SIZE  64
#define STACK_SIZE 64
#define MEM_SIZE   DATA_SIZE+HEAP_SIZE+STACK_SIZE

class BuiltInFunScope;

class Interpreter:public ASTVisitor{
    public:
        class LeftSideAssigner:public ASTVisitor{
            private:
                Interpreter* interpreter;
            public:
                LeftSideAssigner(Interpreter* interpreter);
                void visit(VarAccessExpression* ex)override;
                void visit(NonStaticVarAccessExpression* ex)override;
        };
        class RightSideAssigner:public ASTVisitor{
            private:
                Interpreter* interpreter;
            public:
                RightSideAssigner(Interpreter* interpreter);
                void visit(VarAccessExpression* ex)override;
                void visit(NonStaticVarAccessExpression* ex)override;
        };

        LeftSideAssigner* lAssigner;
        RightSideAssigner* rAssigner;
    private:
        bool funReturn;
        bool loopBreak;
        bool loopContinue;

        void runStmList(StmListScope* scope);
        void reserveSpaceForStmListLocals(int size);
        void removeReservedSpaceForStmListLocals(int size);
        
        void invokeNonStaticFun(NonStaticFunInvokeExpression* ex);
        void invokeNonStaticBuiltInFun(NonStaticFunInvokeExpression* ex);
        void invokeBuiltInOpFun(OperatorFunInvokeExpression* ex);
        void initMultiDArray();
        int getAvailableHeapAddress(ClassScope* scope);
        int getAvailableHeapAddress(int size);
    public:
        Interpreter();
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
        void visit(VoidExpression* ex)override;
        void visit(LogicalExpression* ex)override;
        void visit(NonStaticVarAccessExpression* ex)override;
        void visit(NonStaticFunInvokeExpression* ex)override;
        void visit(OperatorFunInvokeExpression* ex)override;
        void visit(SetOperatorExpression* ex)override;
        void visit(ThisExpression* ex)override;
        void visit(ThisVarAccessExpression* ex)override;
        void visit(ThisFunInvokeExpression* ex)override;

        void push(SharedIValue val);
        SharedIValue top();
        SharedIValue pop();
        template<typename T>
        std::shared_ptr<T> popAs();
        template<typename T>
        std::shared_ptr<T> topAs();

        std::unordered_map<Variable*, VarsOffsetSetter::Offset> offsets;
        SharedIValue AX;
        SharedIValue CX;
        SharedIValue DX;
        int*const DI;
        int*const BX;
        int*const BP;
        int*const SP;
        const int*const DS;
        const int*const SS;
        SharedIValue memory[MEM_SIZE];
};

template<typename T>
inline std::shared_ptr<T> Interpreter::popAs(){
    return std::dynamic_pointer_cast<T>(pop());
}

template<typename T>
inline std::shared_ptr<T> Interpreter::topAs(){
    return std::dynamic_pointer_cast<T>(top());
}