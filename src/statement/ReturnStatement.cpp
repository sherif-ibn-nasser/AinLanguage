#include "ReturnStatement.hpp"
#include "BaseScope.hpp"
#include "FunScope.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"
#include "IExpression.hpp"
#include "UnexpectedTypeException.hpp"
#include "FunDecl.hpp"
#include "VoidExpression.hpp"
#include <memory>

ReturnStatement::ReturnStatement(
    int lineNumber,
    SharedStmListScope runScope,
    SharedIExpression ex
):IStatement(lineNumber,runScope),ex(ex){}

SharedIExpression ReturnStatement::getEx()const{
    return ex;
}