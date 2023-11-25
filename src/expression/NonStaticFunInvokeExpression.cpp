#include "NonStaticFunInvokeExpression.hpp"

NonStaticFunInvokeExpression::NonStaticFunInvokeExpression(
    int lineNumber,
    std::wstring funName,
    SharedVector<SharedIExpression> args,
    SharedIExpression inside
)
    : IExpression(lineNumber,nullptr),
      funName(funName),
      args(args),
      inside(inside){}

NonStaticFunInvokeExpression::~NonStaticFunInvokeExpression(){}

std::wstring NonStaticFunInvokeExpression::getFunName()const{
    return funName;
}

SharedIExpression NonStaticFunInvokeExpression::getInside()const{
    return inside;
}

SharedVector<SharedIExpression> NonStaticFunInvokeExpression::getArgs()const{
    return args;
}

SharedFunScope NonStaticFunInvokeExpression::getFun()const{
    return fun;
}

void NonStaticFunInvokeExpression::setFun(SharedFunScope fun){
    this->fun=fun;
}