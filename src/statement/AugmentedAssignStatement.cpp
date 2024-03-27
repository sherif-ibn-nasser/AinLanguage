#include "AugmentedAssignStatement.hpp"

AugmentedAssignStatement::AugmentedAssignStatement(
    int lineNumber,
    SharedStmListScope runScope,
    Operator op,
    SharedIExpression ex,
    SharedIExpression newValEx
):AssignStatement(lineNumber,runScope,ex,newValEx),op(op)
{}

AugmentedAssignStatement::Operator AugmentedAssignStatement::getOp()const{
    return this->op;
}

SharedFunScope AugmentedAssignStatement::getOpFun()const{
    return opFun;
}

void AugmentedAssignStatement::setOpFun(SharedFunScope opFun){
    this->opFun=opFun;
}

void AugmentedAssignStatement::setOpFunExplicit(bool isOpFunExplicit){
    this->explicitOpFun=isOpFunExplicit;
}

bool AugmentedAssignStatement::isOpFunExplicit(){
    return this->explicitOpFun;
}