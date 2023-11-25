#include "IfStatement.hpp"
#include "BoolValue.hpp"
#include "SharedPtrTypes.hpp"
#include "FunScope.hpp"
#include "IExpression.hpp"
#include "Type.hpp"
#include "UnexpectedTypeException.hpp"

IfStatement::IfStatement(
    int lineNumber,
    SharedStmListScope runScope,
    SharedIExpression ifCondition,
    SharedStmListScope ifScope,
    SharedStmListScope elseScope
)
    : IStatement(lineNumber,runScope),
      ifCondition(ifCondition),
      ifScope(ifScope),
      elseScope(elseScope)
{}

SharedIExpression IfStatement::getIfCondition()const{
    return ifCondition;
}

SharedStmListScope IfStatement::getIfScope()const{
    return ifScope;
}

SharedStmListScope IfStatement::getElseScope()const{
    return elseScope;
}