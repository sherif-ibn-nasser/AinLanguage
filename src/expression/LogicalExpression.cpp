#include "LogicalExpression.hpp"
#include "BoolValue.hpp"
#include "Type.hpp"
#include "UnexpectedTypeException.hpp"
#include <memory>
#include <string>
#include <vector>

LogicalExpression::LogicalExpression(
    int lineNumber,
    Operation logicalOp,
    SharedIExpression left,
    SharedIExpression right
)
    : IExpression(lineNumber,Type::BOOL),
    logicalOp(logicalOp),
    left(left),
    right(right){}

SharedIExpression LogicalExpression::getLeft()const{
    return left;
}

SharedIExpression LogicalExpression::getRight()const{
    return right;
}

LogicalExpression::Operation LogicalExpression::getLogicalOp()const{
    return logicalOp;
}