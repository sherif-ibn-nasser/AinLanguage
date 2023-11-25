#include "AssignStatement.hpp"
#include "SharedPtrTypes.hpp"
#include "StmListScope.hpp"
#include "Type.hpp"
#include "semantics/UnexpectedTypeException.hpp"

AssignStatement::AssignStatement(
    int lineNumber,
    SharedStmListScope runScope,
    SharedIExpression left,
    SharedIExpression right
)
:IStatement(lineNumber,runScope),left(left),right(right){}

SharedIExpression AssignStatement::getLeft()const{
    return left;
}

SharedIExpression AssignStatement::getRight()const{
    return right;
}