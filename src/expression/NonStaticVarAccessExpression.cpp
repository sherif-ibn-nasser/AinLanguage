#include "NonStaticVarAccessExpression.hpp"

NonStaticVarAccessExpression::NonStaticVarAccessExpression(
    int lineNumber,
    std::wstring varName,
    SharedIExpression inside
):
    IExpression(lineNumber,nullptr),
    varName(varName),
    inside(inside){}

std::wstring NonStaticVarAccessExpression::getVarName()const{
    return varName;
}

SharedIExpression NonStaticVarAccessExpression::getInside()const{
    return inside;
}

SharedVariable NonStaticVarAccessExpression::getVar()const{
    return var;
}

void NonStaticVarAccessExpression::setVar(SharedVariable var){
    this->var=var;
}