#include "VoidExpression.hpp"
#include "VoidValue.hpp"
#include <memory>
#include <string>

VoidExpression::VoidExpression(int lineNumber):
IExpression(lineNumber,Type::VOID){}