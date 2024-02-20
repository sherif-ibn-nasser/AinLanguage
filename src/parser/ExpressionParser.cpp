#include "ExpressionParser.hpp"
#include "AssignStatement.hpp"
#include "BoolValue.hpp"
#include "ByteValue.hpp"
#include "CharValue.hpp"
#include "ClassScope.hpp"
#include "DoubleValue.hpp"
#include "ExpressionExpectedException.hpp"
#include "FileScope.hpp"
#include "FloatValue.hpp"
#include "FunInvokeExpression.hpp"
#include "IExpression.hpp"
#include "IntValue.hpp"
#include "KeywordToken.hpp"
#include "LexerToken.hpp"
#include "LiteralExpression.hpp"
#include "LiteralToken.hpp"
#include "LogicalExpression.hpp"
#include "LongValue.hpp"
#include "NewObjectExpression.hpp"
#include "NewArrayExpression.hpp"
#include "NonStaticFunInvokeExpression.hpp"
#include "NonStaticVarAccessExpression.hpp"
#include "NumberToken.hpp"
#include "OnlyVariablesAreAssignableException.hpp"
#include "OperatorFunInvokeExpression.hpp"
#include "OperatorFunctions.hpp"
#include "SetOperatorExpression.hpp"
#include "SharedPtrTypes.hpp"
#include "StmListScope.hpp"
#include "StringValue.hpp"
#include "SymbolToken.hpp"
#include "ThisExpression.hpp"
#include "ThisFunInvokeExpression.hpp"
#include "ThisVarAccessExpression.hpp"
#include "Type.hpp"
#include "UByteValue.hpp"
#include "UIntValue.hpp"
#include "ULongValue.hpp"
#include "TokenIsNotAllowedHereException.hpp"
#include "UnitExpression.hpp"
#include "VarAccessExpression.hpp"
#include <algorithm>
#include <cassert>
#include <memory>
#include <map>
#include <string>
#include <vector>

ExpressionParser::ExpressionParser(
    SharedTokensIterator iterator,
    SharedBaseScope scope,
    TypeParserProvider typeParserProvider
):BaseParser(iterator,scope),typeParserProvider(typeParserProvider){}

SharedIExpression ExpressionParser::parse(){
    prevLineNumber=iterator->lineNumber;
    return parseBinaryOperatorExpression();
}

SharedIExpression ExpressionParser::parseBinaryOperatorExpression(int precedence){
    if(precedence==0)
        return parsePrimaryExpression();
    
    auto left=parseBinaryOperatorExpression(precedence-1);

    int lineNumber=iterator->lineNumber;

    auto op=iterator->currentToken();

    while(currentMatchByPrecedence(precedence)){
        lineNumber=iterator->lineNumber;

        op=iterator->currentToken();

        next();

        if(*op==SymbolToken::LOGICAL_OR){
            auto right=parseBinaryOperatorExpression(precedence);
            if(!right)
                throw ExpressionExpectedException(iterator->lineNumber);
            return std::make_shared<LogicalExpression>(
                lineNumber,LogicalExpression::Operation::OR,left,right
            );
        }

        if(*op==SymbolToken::LOGICAL_AND){
            auto right=parseBinaryOperatorExpression(precedence);
            if(!right)
                throw ExpressionExpectedException(iterator->lineNumber);
            return std::make_shared<LogicalExpression>(
                lineNumber,LogicalExpression::Operation::AND,left,right
            );
        }

        auto right=parseBinaryOperatorExpression(precedence-1);

        if(!right)
            throw ExpressionExpectedException(iterator->lineNumber);

        auto args=std::make_shared<std::vector<SharedIExpression>>(std::vector({right}));

        left=std::make_shared<OperatorFunInvokeExpression>(
            lineNumber,
            getBinOpFromToken(*op),
            args,
            left
        );
        
    }

    return left;
}

SharedIExpression ExpressionParser::parsePrimaryExpression(){
    
    if(auto unaryOpEx=parseUnaryOperatorExpression())
        return unaryOpEx;
    
    SharedIExpression primary;

    if(auto parenthesesEx=parseParenthesesExpression())
        primary=parenthesesEx;

    else if(auto literalEx=parseLiteralExpression())
        primary=literalEx;

    else if(auto idEx=parseIdentifierExpression())
        primary=idEx;

    else if(auto newObjEx=parseNewExpression())
        primary=newObjEx;

    else if(auto thisEx=parseThisExpression())
        primary=thisEx;

    else
        return nullptr;

    return parseNonStaticAccessExpression(primary);
    
}

SharedIExpression ExpressionParser::parseUnaryOperatorExpression(){

    OperatorFunInvokeExpression::Operator unaryOp;

    if(iterator->currentMatch(SymbolToken::PLUS))
        unaryOp=OperatorFunInvokeExpression::Operator::UNARY_PLUS;

    else if(iterator->currentMatch(SymbolToken::MINUS))
        unaryOp=OperatorFunInvokeExpression::Operator::UNARY_MINUS;

    else if(iterator->currentMatch(SymbolToken::EXCLAMATION_MARK))
        unaryOp=OperatorFunInvokeExpression::Operator::LOGICAL_NOT;

    else if(iterator->currentMatch(SymbolToken::BIT_NOT))
        unaryOp=OperatorFunInvokeExpression::Operator::BIT_NOT;

    else if(iterator->currentMatch(SymbolToken::PLUS_PLUS))
        unaryOp=OperatorFunInvokeExpression::Operator::PRE_INC;

    else if(iterator->currentMatch(SymbolToken::MINUS_MINUS))
        unaryOp=OperatorFunInvokeExpression::Operator::PRE_DEC;

    else
        return nullptr;
    
    auto lineNumber=iterator->lineNumber;

    next();

    auto primary=parsePrimaryExpression();
    
    if(  
        unaryOp==OperatorFunInvokeExpression::Operator::PRE_INC
        ||
        unaryOp==OperatorFunInvokeExpression::Operator::PRE_DEC
    ){
        auto primaryAsGetEx=IExpression::isGetOpFunInvokeExpression(primary);

        if(primaryAsGetEx){
            auto setSubOp=
                (unaryOp==OperatorFunInvokeExpression::Operator::PRE_INC)
                ?SetOperatorExpression::Operator::PRE_INC
                :SetOperatorExpression::Operator::PRE_DEC
            ;
            return std::make_shared<SetOperatorExpression>(
                setSubOp,
                primaryAsGetEx,
                nullptr
            );
        }

        if(!IExpression::isAssignableExpression(primary))
            throw OnlyVariablesAreAssignableException(lineNumber);
    }

    auto args=std::make_shared<std::vector<SharedIExpression>>(
        std::vector<SharedIExpression>{}
    );
    
    return std::make_shared<OperatorFunInvokeExpression>(
        lineNumber,
        unaryOp,
        args,
        primary
    );
    
}

SharedIExpression ExpressionParser::parseParenthesesExpression(){
    
    if(!iterator->currentMatch(SymbolToken::LEFT_PARENTHESIS))
        return nullptr;
    
    next();

    auto ex=parse();

    expectSymbol(SymbolToken::RIGHT_PARENTHESIS);

    next();

    return ex;
}

SharedIExpression ExpressionParser::parseLiteralExpression(){

    int lineNumber=iterator->lineNumber;

    if(iterator->currentMatch(KeywordToken::TRUE)){
        next();
        auto value=std::make_shared<BoolValue>(true);
        return std::make_shared<LiteralExpression>(lineNumber,value);
    }

    if(iterator->currentMatch(KeywordToken::FALSE)){
        next();
        auto value=std::make_shared<BoolValue>(false);
        return std::make_shared<LiteralExpression>(lineNumber,value);
    }
        
    
    auto literal=std::dynamic_pointer_cast<LiteralToken>(iterator->currentToken());
    
    if(!literal)
        return nullptr;
    
    SharedIValue value;

    switch(literal->getLiteralType()){

        case LiteralToken::STRING:{
            auto val=literal->getVal();
            value=std::make_shared<StringValue>(
                val.substr(1,val.size()-2) // remove quotes
            );
            break;
        }

        case LiteralToken::CHAR:{
            value=std::make_shared<CharValue>(
                literal->getVal().at(1) // remove quotes
            );
            break;
        }

        case LiteralToken::NUMBER:{
            auto numType=std::dynamic_pointer_cast<NumberToken>(literal)->getNumberType();
            value=parseNumberValue(numType,literal->getVal());
            break;
        }

        default:{}
    }

    next();

    return std::make_shared<LiteralExpression>(lineNumber,value);
}

SharedIExpression ExpressionParser::parseIdentifierExpression(){
    
    if(iterator->currentTokenType()!=LexerToken::IDENTIFIER_TOKEN)
        return nullptr;
    
    auto lineNumber=iterator->lineNumber;
    
    auto id=iterator->currentVal();

    next();

    if(currentMatchInSameLine(SymbolToken::LEFT_PARENTHESIS)){
        auto args=expectFunArgs();
        
        auto funEx=std::make_shared<FunInvokeExpression>(
            lineNumber,
            id,
            args
        );
        return funEx;
    }

    SharedVariable var;

    if(auto stmListScope=BaseScope::toStmListScope(scope))
        var=stmListScope->getLocalByName(id);
    else if(auto classScope=BaseScope::toClassScope(scope)){
        var=classScope->findPublicVariable(id);
        if(!var)
            var=classScope->findPrivateVariable(id);
    }
    else if(auto fileScope=BaseScope::toFileScope(scope)){
        var=fileScope->findPublicVariable(id);
        if(!var)
            var=fileScope->findPrivateVariable(id);
    }

    auto varEx=std::make_shared<VarAccessExpression>(lineNumber,id,var);

    return varEx;
    
}

SharedIExpression ExpressionParser::parseNewExpression(){

    if(!iterator->currentMatch(KeywordToken::NEW))
        return nullptr;
    
    int lineNumber=iterator->lineNumber;

    next();

    auto type=typeParserProvider(iterator,scope)->parse();

    // The capacity expressions for a multi-dimensional array
    auto arraysCapacities=std::vector<SharedIExpression>();

    while(iterator->currentMatch(SymbolToken::LEFT_SQUARE_BRACKET)){
        next();

        auto ex=parse();

        if(!ex)
            throw ExpressionExpectedException(iterator->lineNumber);

        arraysCapacities.push_back(ex);
        
        expectSymbol(SymbolToken::RIGHT_SQUARE_BRACKET);

        next();
    }

    // ANCHOR: Do we need to specify maximum dimension of an array?
    if(!arraysCapacities.empty())
        return std::make_shared<NewArrayExpression>(
            lineNumber,
            arraysCapacities,
            type
        );
    
    auto args=expectFunArgs();

    auto newObjEx=std::make_shared<NewObjectExpression>(
        lineNumber,
        type,
        args
    );

    return newObjEx;
}

SharedIExpression ExpressionParser::parseThisExpression(){
    
    if(!iterator->currentMatch(KeywordToken::THIS))
        return nullptr;

    auto lineNumber=iterator->lineNumber;

    auto classScope=BaseScope::getContainingClass(scope);

    if(!classScope)
        throw TokenIsNotAllowedHereException(
            lineNumber,
            KeywordToken::THIS.getVal()
        );

    if(!iterator->nextMatch(SymbolToken::DOT))
        return std::make_shared<ThisExpression>(lineNumber,classScope);

    lineNumber=iterator->lineNumber;

    auto id=expectNextIdentifier();

    next();

    if(currentMatchInSameLine(SymbolToken::LEFT_PARENTHESIS)){
        auto args=expectFunArgs();
        
        auto funEx=std::make_shared<ThisFunInvokeExpression>(
            lineNumber,
            classScope,
            id,
            args
        );

        return funEx;
    }

    auto varEx=std::make_shared<ThisVarAccessExpression>(
        lineNumber,
        classScope,
        id
    );

    return varEx;

}

SharedIExpression ExpressionParser::parseNonStaticAccessExpression(SharedIExpression inside){

    auto ex=parsePostOpExpression(inside);

    if(!iterator->currentMatch(SymbolToken::DOT))
        return ex;
    
    inside=ex;
    
    next();
    
    int lineNumber=iterator->lineNumber;

    // TODO: right might be inner class instantiation

    auto id=expectIdentifier();

    next();
    
    if(currentMatchInSameLine(SymbolToken::LEFT_PARENTHESIS))
        ex=std::make_shared<NonStaticFunInvokeExpression>(
            lineNumber,
            id,
            expectFunArgs(),
            inside
        );
    else
        ex=std::make_shared<NonStaticVarAccessExpression>(
            lineNumber,
            id,
            inside
        );
    
    return parseNonStaticAccessExpression(ex);
}

SharedIExpression ExpressionParser::parsePostOpExpression(SharedIExpression inside){

    OperatorFunInvokeExpression::Operator op;

    auto lineNumber=iterator->lineNumber;

    if(currentMatchInSameLine(SymbolToken::PLUS_PLUS))
        op=OperatorFunInvokeExpression::Operator::POST_INC;

    else if(currentMatchInSameLine(SymbolToken::MINUS_MINUS))
        op=OperatorFunInvokeExpression::Operator::POST_DEC;

    else if(currentMatchInSameLine(SymbolToken::LEFT_SQUARE_BRACKET)){
        op=OperatorFunInvokeExpression::Operator::GET;
        next();
        auto indexEx=parse();
        expectSymbol(SymbolToken::RIGHT_SQUARE_BRACKET);
        next();

        auto args=std::make_shared<std::vector<SharedIExpression>>(
            std::vector<SharedIExpression>{indexEx}
        );
        
        auto getEx=std::make_shared<OperatorFunInvokeExpression>(
            lineNumber,
            op,
            args,
            inside
        );

        return parsePostOpExpression(getEx);
    }
    else
        return inside;

    next();

    auto insideAsGetEx=IExpression::isGetOpFunInvokeExpression(inside);

    if(insideAsGetEx){
        auto setSubOp=
            (op==OperatorFunInvokeExpression::Operator::POST_INC)
            ?SetOperatorExpression::Operator::POST_INC
            :SetOperatorExpression::Operator::POST_DEC
        ;
        return std::make_shared<SetOperatorExpression>(
            setSubOp,
            insideAsGetEx,
            nullptr
        );
    }

    if(!IExpression::isAssignableExpression(inside))
        throw OnlyVariablesAreAssignableException(lineNumber);

    auto args=std::make_shared<std::vector<SharedIExpression>>(
        std::vector<SharedIExpression>{}
    );
    
    return std::make_shared<OperatorFunInvokeExpression>(
        lineNumber,
        op,
        args,
        inside
    );
    
}

bool ExpressionParser::currentMatchByPrecedence(int precedence){
    switch(precedence){
        case 1:
            return currentMatchInSameLine(SymbolToken::POWER);
        case 2:
            return
                currentMatchInSameLine(SymbolToken::STAR)
                ||
                currentMatchInSameLine(SymbolToken::SLASH)
                ||
                currentMatchInSameLine(SymbolToken::MODULO)
            ;
        case 3:
            return
                currentMatchInSameLine(SymbolToken::PLUS)
                ||
                currentMatchInSameLine(SymbolToken::MINUS)
            ;
        case 4:
            return
                currentMatchInSameLine(SymbolToken::GREATER_EQUAL)
                ||
                currentMatchInSameLine(SymbolToken::LESS_EQUAL)
                ||
                currentMatchInSameLine(SymbolToken::LEFT_ANGLE_BRACKET)
                ||
                currentMatchInSameLine(SymbolToken::RIGHT_ANGLE_BRACKET)
            ;
        case 5:
            return
                currentMatchInSameLine(SymbolToken::EQUAL_EQUAL)
                ||
                currentMatchInSameLine(SymbolToken::NOT_EQUAL)
            ;
        case 6:
            return
                currentMatchInSameLine(SymbolToken::SHR)
                ||
                currentMatchInSameLine(SymbolToken::SHL)
            ;
        case 7:
            return currentMatchInSameLine(SymbolToken::AMPERSAND);
        case 8:
            return currentMatchInSameLine(SymbolToken::XOR);
        case 9:
            return currentMatchInSameLine(SymbolToken::BAR);
        case 10:
            return iterator->currentMatch(SymbolToken::LOGICAL_AND);
        case 11:
            return iterator->currentMatch(SymbolToken::LOGICAL_OR);
    }
    return false;
}

OperatorFunInvokeExpression::Operator ExpressionParser::getBinOpFromToken(LexerToken op){
        
    if(op==SymbolToken::PLUS)
        return OperatorFunInvokeExpression::Operator::PLUS;
    if(op==SymbolToken::MINUS)
        return OperatorFunInvokeExpression::Operator::MINUS;
    if(op==SymbolToken::STAR)
        return OperatorFunInvokeExpression::Operator::TIMES;
    if(op==SymbolToken::SLASH)
        return OperatorFunInvokeExpression::Operator::DIV;
    if(op==SymbolToken::MODULO)
        return OperatorFunInvokeExpression::Operator::MOD;
    if(op==SymbolToken::POWER)
        return OperatorFunInvokeExpression::Operator::POW;
    if(op==SymbolToken::EQUAL_EQUAL)
        return OperatorFunInvokeExpression::Operator::EQUAL_EQUAL;
    if(op==SymbolToken::NOT_EQUAL)
        return OperatorFunInvokeExpression::Operator::NOT_EQUAL;
    if(op==SymbolToken::LEFT_ANGLE_BRACKET)
        return OperatorFunInvokeExpression::Operator::LESS;
    if(op==SymbolToken::LESS_EQUAL)
        return OperatorFunInvokeExpression::Operator::LESS_EQUAL;
    if(op==SymbolToken::RIGHT_ANGLE_BRACKET)
        return OperatorFunInvokeExpression::Operator::GREATER;
    if(op==SymbolToken::GREATER_EQUAL)
        return OperatorFunInvokeExpression::Operator::GREATER_EQUAL;
    if(op==SymbolToken::SHL)
        return OperatorFunInvokeExpression::Operator::SHL;
    if(op==SymbolToken::SHR)
        return OperatorFunInvokeExpression::Operator::SHR;
    if(op==SymbolToken::AMPERSAND)
        return OperatorFunInvokeExpression::Operator::BIT_AND;
    if(op==SymbolToken::XOR)
        return OperatorFunInvokeExpression::Operator::XOR;
    if(op==SymbolToken::BAR)
        return OperatorFunInvokeExpression::Operator::BIT_OR;
    
    assert(false); // Unreachable
}

SharedIValue ExpressionParser::parseNumberValue(NumberToken::NUMBER_TYPE numType,std::wstring value) {
    switch(numType){
        case NumberToken::BYTE:
            return std::make_shared<ByteValue>(
                std::stoi(value)
            );

        case NumberToken::UNSIGNED_BYTE:
            return std::make_shared<UByteValue>(
                std::stoul(value)
            );

        case NumberToken::INT:
            return std::make_shared<IntValue>(
                std::stoi(value)
            );

        case NumberToken::UNSIGNED_INT:
            return std::make_shared<UIntValue>(
                std::stoul(value)
            );

        case NumberToken::LONG:
            return std::make_shared<LongValue>(
                std::stoll(value)
            );

        case NumberToken::UNSIGNED_LONG:
            return std::make_shared<ULongValue>(
                std::stoull(value)
            );

        case NumberToken::DOUBLE:
            return std::make_shared<DoubleValue>(
                std::stod(value)
            );

        case NumberToken::FLOAT:
            return std::make_shared<FloatValue>(
                std::stof(value)
            );
    }

    // To stop compiler warnings
    return nullptr;
}

SharedVector<SharedIExpression> ExpressionParser::expectFunArgs(){

    expectSymbol(SymbolToken::LEFT_PARENTHESIS);

    SharedVector<SharedIExpression> args=std::make_shared<std::vector<SharedIExpression>>();

    do{

        if(nextMatch(SymbolToken::RIGHT_PARENTHESIS))
            break;

        auto arg=parse();

        args->push_back(arg);

    }while(iterator->currentMatch(SymbolToken::COMMA));

    expectSymbol(SymbolToken::RIGHT_PARENTHESIS);

    next();

    return args;
}

void ExpressionParser::next(){
    prevLineNumber=iterator->lineNumber;
    iterator->next();
}

bool ExpressionParser::nextMatch(LexerToken expected){
    prevLineNumber=iterator->lineNumber;
    return iterator->nextMatch(expected);
}

bool ExpressionParser::currentMatchInSameLine(LexerToken expected){
    return
        prevLineNumber==iterator->lineNumber
        &&
        iterator->currentMatch(expected)
    ;
}