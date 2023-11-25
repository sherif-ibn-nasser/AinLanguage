#include "VarAccessExpression.hpp"
#include "BaseScope.hpp"
#include "ClassScope.hpp"
#include "FileScope.hpp"
#include "IExpression.hpp"
#include "PackageScope.hpp"
#include "VariableNotFoundException.hpp"
#include "MustHaveExplicitTypeException.hpp"
#include "Variable.hpp"
#include <string>

VarAccessExpression::VarAccessExpression(
    int lineNumber,
    std::wstring varName,
    SharedVariable var
):
IExpression(lineNumber,(var)?var->getType():nullptr),
varName(varName),
var(var)
{}

SharedVariable VarAccessExpression::getVar()const{
    return var;
}

std::wstring VarAccessExpression::getVarName()const{
    return varName;
}

void VarAccessExpression::setVar(SharedVariable var){
    this->var=var;
}