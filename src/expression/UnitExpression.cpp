#include "UnitExpression.hpp"
#include "UnitValue.hpp"
#include <memory>
#include <string>

UnitExpression::UnitExpression(int lineNumber):
IExpression(lineNumber,Type::UNIT){}