#include "ExpressionStatement.hpp"
#include "StmListScope.hpp"
#include "IExpression.hpp"

ExpressionStatement::ExpressionStatement(
    int lineNumber,
    SharedStmListScope runScope,
    SharedIExpression ex
)
: IStatement(lineNumber,runScope),ex(ex){}

SharedIExpression ExpressionStatement::getEx()const{
    return ex;
}