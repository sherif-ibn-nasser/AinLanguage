#include "OperatorFunInvokeExpression.hpp"
#include "FunctionNotFoundException.hpp"
#include "LexerToken.hpp"
#include "NonStaticFunInvokeExpression.hpp"
#include "FunScope.hpp"
#include "FunDecl.hpp"
#include "OperatorFunctions.hpp"
#include "Type.hpp"
#include "ClassScope.hpp"
#include "FunParam.hpp"

OperatorFunInvokeExpression::OperatorFunInvokeExpression(
    int lineNumber,
    Operator op,
    SharedVector<SharedIExpression> args,
    SharedIExpression inside
):
NonStaticFunInvokeExpression(
    lineNumber,
    L"",
    args,
    inside
),op(op){
    switch(op){
        case Operator::PLUS:
            this->funName=OperatorFunctions::PLUS_NAME;break;
        case Operator::MINUS:
            this->funName=OperatorFunctions::MINUS_NAME;break;
        case Operator::TIMES:
            this->funName=OperatorFunctions::TIMES_NAME;break;
        case Operator::DIV:
            this->funName=OperatorFunctions::DIV_NAME;break;
        case Operator::MOD:
            this->funName=OperatorFunctions::MOD_NAME;break;
        case Operator::POW:
            this->funName=OperatorFunctions::POW_NAME;break;
        case Operator::EQUAL_EQUAL:
        case Operator::NOT_EQUAL:
            this->funName=OperatorFunctions::EQUALS_NAME;break;
        case Operator::LESS:
        case Operator::LESS_EQUAL:
        case Operator::GREATER:
        case Operator::GREATER_EQUAL:
            this->funName=OperatorFunctions::COMPARE_TO_NAME;break;
        case Operator::SHR:
            this->funName=OperatorFunctions::SHR_NAME;break;
        case Operator::SHL:
            this->funName=OperatorFunctions::SHL_NAME;break;
        case Operator::BIT_AND:
            this->funName=OperatorFunctions::BIT_AND_NAME;break;
        case Operator::XOR:
            this->funName=OperatorFunctions::XOR_NAME;break;
        case Operator::BIT_OR:
            this->funName=OperatorFunctions::BIT_OR_NAME;break;
        case Operator::UNARY_PLUS:
            this->funName=OperatorFunctions::UNARY_PLUS_NAME;break;
        case Operator::UNARY_MINUS:
            this->funName=OperatorFunctions::UNARY_MINUS_NAME;break;
        case Operator::LOGICAL_NOT:
            this->funName=OperatorFunctions::LOGICAL_NOT_NAME;break;
        case Operator::BIT_NOT:
            this->funName=OperatorFunctions::BIT_NOT_NAME;break;
        case Operator::PRE_INC:
        case Operator::POST_INC:
            this->funName=OperatorFunctions::INC_NAME;break;
        case Operator::PRE_DEC:
        case Operator::POST_DEC:
            this->funName=OperatorFunctions::DEC_NAME;break;
        case Operator::GET:
            this->funName=OperatorFunctions::GET_NAME;break;
        case Operator::SET_EQUAL:
            this->funName=OperatorFunctions::SET_NAME;break;
    }
}

OperatorFunInvokeExpression::Operator OperatorFunInvokeExpression::getOp()const{
    return op;
}