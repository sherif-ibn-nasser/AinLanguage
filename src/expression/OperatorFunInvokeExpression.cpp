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
#include "string_helper.hpp"
#include <stdexcept>

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

OperatorFunInvokeExpression::OperatorFunInvokeExpression(
    int lineNumber,
    std::wstring opName,
    SharedVector<SharedIExpression> args,
    SharedIExpression inside
):
NonStaticFunInvokeExpression(lineNumber, opName, args, inside){
    if(opName==OperatorFunctions::PLUS_NAME){
        this->op=Operator::PLUS;
    }
    else if(opName==OperatorFunctions::MINUS_NAME){
        this->op=Operator::MINUS;
    }
    else if(opName==OperatorFunctions::TIMES_NAME){
        this->op=Operator::TIMES;
    }
    else if(opName==OperatorFunctions::DIV_NAME){
        this->op=Operator::DIV;
    }
    else if(opName==OperatorFunctions::MOD_NAME){
        this->op=Operator::MOD;
    }
    else if(opName==OperatorFunctions::POW_NAME){
        this->op=Operator::POW;
    }
    else if(opName==OperatorFunctions::COMPARE_TO_NAME){
        this->op=Operator::LESS;
    }
    else if(opName==OperatorFunctions::EQUALS_NAME){
        this->op=Operator::EQUAL_EQUAL;
    }
    else if(opName==OperatorFunctions::SHR_NAME){
        this->op=Operator::SHR;
    }
    else if(opName==OperatorFunctions::SHL_NAME){
        this->op=Operator::SHL;
    }
    else if(opName==OperatorFunctions::BIT_AND_NAME){
        this->op=Operator::BIT_AND;
    }
    else if(opName==OperatorFunctions::XOR_NAME){
        this->op=Operator::XOR;
    }
    else if(opName==OperatorFunctions::BIT_OR_NAME){
        this->op=Operator::BIT_OR;
    }
    else if(opName==OperatorFunctions::LOGICAL_NOT_NAME){
        this->op=Operator::LOGICAL_NOT;
    }
    else if(opName==OperatorFunctions::BIT_NOT_NAME){
        this->op=Operator::BIT_NOT;
    }
    else if(opName==OperatorFunctions::UNARY_PLUS_NAME){
        this->op=Operator::UNARY_PLUS;
    }
    else if(opName==OperatorFunctions::UNARY_MINUS_NAME){
        this->op=Operator::UNARY_MINUS;
    }
    else if(opName==OperatorFunctions::INC_NAME){
        this->op=Operator::PRE_INC;
    }
    else if(opName==OperatorFunctions::DEC_NAME){
        this->op=Operator::PRE_DEC;
    }
    else if(opName==OperatorFunctions::GET_NAME){
        this->op=Operator::GET;
    }
    else if(opName==OperatorFunctions::SET_NAME){
        this->op=Operator::SET_EQUAL;
    }
    else
        throw std::invalid_argument(toCharPointer(L"Cannot instantiate an operator fun name with name \""+opName+L"\""));
}

OperatorFunInvokeExpression::Operator OperatorFunInvokeExpression::getOp()const{
    return op;
}