#include "NewObjectExpression.hpp"
#include "AinException.hpp"
#include "FileScope.hpp"
#include "FunDecl.hpp"
#include "FunInvokeExpression.hpp"
#include "FunParam.hpp"
#include "FunctionNotFoundException.hpp"
#include "IExpression.hpp"
#include "SharedPtrTypes.hpp"
#include "ClassScope.hpp"
#include "Type.hpp"
#include <memory>
#include <string>
#include <vector>

NewObjectExpression::NewObjectExpression(
    int lineNumber,
    SharedType type,
    SharedVector<SharedIExpression> args
):IExpression(lineNumber,type),args(args){}

SharedVector<SharedIExpression> NewObjectExpression::getArgs()const{
    return args;
}

SharedFunScope NewObjectExpression::getConstructor()const{
    return constructor;
}

void NewObjectExpression::setConstructor(SharedFunScope constructor){
    this->constructor=constructor;
}