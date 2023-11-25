#include "IExpression.hpp"
#include "ThisVarAccessExpression.hpp"
#include "VarAccessExpression.hpp"
#include "NonStaticVarAccessExpression.hpp"
#include "OperatorFunInvokeExpression.hpp"

IExpression::IExpression(int lineNumber,SharedType returnType)
:ASTNode(lineNumber),returnType(returnType){}

SharedType IExpression::getReturnType() {
    return this->returnType;
}

void IExpression::setReturnType(SharedType returnType){
    this->returnType=returnType;
}

IExpression::~IExpression(){}

bool IExpression::isAssignableExpression(SharedIExpression ex){
    return
        std::dynamic_pointer_cast<VarAccessExpression>(ex)
        ||
        std::dynamic_pointer_cast<NonStaticVarAccessExpression>(ex)
        ||
        std::dynamic_pointer_cast<ThisVarAccessExpression>(ex)
    ;
}

SharedOpFunInvokeExpression IExpression::isGetOpFunInvokeExpression(SharedIExpression ex){
    auto opFun=std::dynamic_pointer_cast<OperatorFunInvokeExpression>(ex);
    if(!opFun)
        return nullptr;
    if(opFun->getOp()==OperatorFunInvokeExpression::Operator::GET)
        return opFun;
    return nullptr;
}