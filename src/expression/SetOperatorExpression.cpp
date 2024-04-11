#include "SetOperatorExpression.hpp"
#include "OperatorFunInvokeExpression.hpp"

SetOperatorExpression::SetOperatorExpression(
    Operator op,
    SharedOpFunInvokeExpression exOfGet,
    SharedIExpression valueEx
)
:IExpression(exOfGet->getLineNumber(),nullptr),
    exOfGet(exOfGet),
    valueEx(valueEx),
    op(op)
{}

SharedIExpression SetOperatorExpression::getExHasGetOp()const{
    return this->exOfGet->getInside();
}

SharedOpFunInvokeExpression SetOperatorExpression::getExOfGet()const{
    return this->exOfGet;
}

SharedIExpression SetOperatorExpression::getIndexEx()const{
    return this->exOfGet->getArgs()->at(0);
}

SharedIExpression SetOperatorExpression::getValueEx()const{
    return this->valueEx;
}

SharedFunScope SetOperatorExpression::getFunOfSet()const {
    return this->funOfSet;
}

void SetOperatorExpression::setFunOfSet(SharedFunScope fun){
    this->funOfSet=fun;
}

SharedFunScope SetOperatorExpression::getFunOfOp()const{
    return this->funOfOp;
}

void SetOperatorExpression::setFunOfOp(SharedFunScope fun){
    this->funOfOp=fun;
}

SetOperatorExpression::Operator SetOperatorExpression::getOp()const{
    return this->op;
}

void SetOperatorExpression::setOpFunExplicit(bool isOpFunExplicit){
    this->explicitOpFun=isOpFunExplicit;
}

bool SetOperatorExpression::isOpFunExplicit(){
    return this->explicitOpFun;
}