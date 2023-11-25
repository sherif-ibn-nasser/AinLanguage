#include "ThisFunInvokeExpression.hpp"

ThisFunInvokeExpression::ThisFunInvokeExpression(
    int lineNumber,
    SharedClassScope classScope,
    std::wstring funName,
    SharedVector<SharedIExpression> args
)
    : IExpression(lineNumber,nullptr),
      funName(funName),
      args(args),
      classScope(classScope){}

ThisFunInvokeExpression::~ThisFunInvokeExpression(){}

std::wstring ThisFunInvokeExpression::getFunName()const{
    return funName;
}

SharedClassScope ThisFunInvokeExpression::getClassScope()const{
    return classScope;
}

SharedVector<SharedIExpression> ThisFunInvokeExpression::getArgs()const{
    return args;
}

SharedFunScope ThisFunInvokeExpression::getFun()const{
    return fun;
}

void ThisFunInvokeExpression::setFun(SharedFunScope fun){
    this->fun=fun;
}