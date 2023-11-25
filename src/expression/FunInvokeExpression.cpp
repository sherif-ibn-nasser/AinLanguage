#include "FunInvokeExpression.hpp"
#include "BaseScope.hpp"
#include "FunDecl.hpp"
#include "FunParam.hpp"
#include "FunScope.hpp"
#include "ClassScope.hpp"
#include "FileScope.hpp"
#include "FunctionNotFoundException.hpp"
#include "PackageScope.hpp"
#include "SharedPtrTypes.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

FunInvokeExpression::FunInvokeExpression(int lineNumber,std::wstring funName, SharedVector<SharedIExpression> args)
    : IExpression(lineNumber,nullptr),
      funName(funName),
      args(args){}

std::wstring FunInvokeExpression::getFunName()const{
    return funName;
}

SharedVector<SharedIExpression> FunInvokeExpression::getArgs()const{
    return args;
}

SharedFunScope FunInvokeExpression::getFun() const{
    return fun;
}

void FunInvokeExpression::setFun(SharedFunScope fun){
    this->fun=fun;
}