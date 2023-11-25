#include "WhileStatement.hpp"
#include "BoolValue.hpp"
#include "StmListScope.hpp"
#include "IExpression.hpp"
#include "Type.hpp"
#include "LoopScope.hpp"
#include "UnexpectedTypeException.hpp"

WhileStatement::WhileStatement(
    int lineNumber,
    SharedStmListScope runScope,
    SharedIExpression condition,
    SharedLoopScope loopScope
)
    : IStatement(lineNumber,runScope),
      condition(condition),
      loopScope(loopScope)
{}

SharedIExpression WhileStatement::getCondition()const{
    return condition;
}

SharedLoopScope WhileStatement::getLoopScope()const{
    return loopScope;
}