#include "ThisVarAccessExpression.hpp"

ThisVarAccessExpression::ThisVarAccessExpression(
    int lineNumber,
    SharedClassScope classScope,
    std::wstring varName
):
    IExpression(lineNumber,nullptr),
    varName(varName),
    classScope(classScope){}

std::wstring ThisVarAccessExpression::getVarName()const{
    return varName;
}

SharedClassScope ThisVarAccessExpression::getClassScope()const{
    return classScope;
}

SharedVariable ThisVarAccessExpression::getVar()const{
    return var;
}

void ThisVarAccessExpression::setVar(SharedVariable var){
    this->var=var;
}