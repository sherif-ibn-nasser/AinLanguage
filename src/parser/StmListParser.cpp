#include "StmListParser.hpp"
#include "ASTVisitor.hpp"
#include "AugmentedAssignStatement.hpp"
#include "BreakStatement.hpp"
#include "ConflictingDeclarationException.hpp"
#include "ContinueStatement.hpp"
#include "DoWhileStatement.hpp"
#include "ExpressionExpectedException.hpp"
#include "ExpressionStatement.hpp"
#include "SetOperatorExpression.hpp"
#include "IfStatement.hpp"
#include "KeywordToken.hpp"
#include "LexerToken.hpp"
#include "LoopScope.hpp"
#include "OnlyVariablesAreAssignableException.hpp"
#include "OperatorFunInvokeExpression.hpp"
#include "OperatorFunctions.hpp"
#include "ParserProvidersAliases.hpp"
#include "ReturnStatement.hpp"
#include "SharedPtrTypes.hpp"
#include "StatementExpectedException.hpp"
#include "StmShouldBeUsedInsideLoopException.hpp"
#include "SymbolToken.hpp"
#include "StmListScope.hpp"
#include "Type.hpp"
#include "UnexpectedTokenException.hpp"
#include "VoidExpression.hpp"
#include "VarStm.hpp"
#include "Variable.hpp"
#include "WhileStatement.hpp"
#include <cassert>
#include <memory>
#include <vector>

StmListParser::StmListParser(
    SharedTokensIterator iterator,
    SharedStmListScope scope,
    VarStmParserProvider varStmParserProvider,
    ExpressionParserProvider expressionParserProvider
)
: BaseParser(iterator,scope),
varStmParserProvider(varStmParserProvider),
expressionParserProvider(expressionParserProvider)
{}

SharedStmList StmListParser::parse(){
    
    auto stmListScope=std::dynamic_pointer_cast<StmListScope>(scope);

    parseInScope(stmListScope);

    return stmListScope->getStmList();
}

void StmListParser::parseInScope(SharedStmListScope stmListScope){

    expectSymbol(SymbolToken::LEFT_CURLY_BRACES);
    iterator->next();

    auto stmList=stmListScope->getStmList();
    
    while(iterator->currentTokenType()!=LexerToken::EOF_TOKEN){

        if(iterator->currentMatch(SymbolToken::RIGHT_CURLY_BRACES))
            break;
        
        auto nextStm=parseNextStatement(stmListScope);
        if(nextStm)
            stmListScope->getStmList()->push_back(nextStm);
    }

    expectSymbol(SymbolToken::RIGHT_CURLY_BRACES);

    iterator->next();
}

void StmListParser::parseEvenWithNoCurlyBracesInScope(SharedStmListScope stmListScope){

    if(iterator->currentMatch(SymbolToken::LEFT_CURLY_BRACES)){
        parseInScope(stmListScope);
        return;
    }

    auto nextStm=parseNextStatement(stmListScope);

    if(!nextStm)
        throw StatementExpectedException(iterator->lineNumber);
    
    stmListScope->getStmList()->push_back(nextStm);
}

SharedIStatement StmListParser::parseNextStatement(SharedStmListScope parentScope){

    auto varStm=parseVarStatement(parentScope);
    if(varStm)
        return varStm;

    auto ifStm=parseIfStatement(parentScope);
    if(ifStm)
        return ifStm;

    auto whileStm=parseWhileStatement(parentScope);
    if(whileStm)
        return whileStm;

    auto doWhileStm=parseDoWhileStatement(parentScope);
    if(doWhileStm)
        return doWhileStm;

    auto returnStm=parseReturnStatement(parentScope);
    if(returnStm)
        return returnStm;

    auto breakContinueStm=parseBreakContinueStatement(parentScope);
    if(breakContinueStm)
        return breakContinueStm;

    auto exStm=parseExpressionStatement(parentScope);

    return exStm;

}

SharedIStatement StmListParser::parseVarStatement(SharedStmListScope parentScope) {
    int lineNumber=iterator->lineNumber;

    auto varStm=varStmParserProvider(iterator,parentScope)->parse();

    if(!varStm)
        return nullptr;
    
    auto var=varStm->getVar();
    auto varName=*var->getName();
    auto locals=parentScope->getLocals();

    if (locals->find(varName)!=locals->end())
        throw ConflictingDeclarationException(lineNumber);

    (*locals)[varName]=var;
    
    return varStm;
}

SharedIStatement StmListParser::parseIfStatement(SharedStmListScope parentScope){

    if(!iterator->currentMatch(KeywordToken::IF))
        return nullptr;

    int lineNumber=iterator->lineNumber;

    expectNextSymbol(SymbolToken::LEFT_PARENTHESIS);
    
    iterator->next();

    auto conditionEx=expressionParserProvider(iterator,parentScope)->parse();

    if(!conditionEx)
        throw ExpressionExpectedException(iterator->lineNumber);
    
    expectSymbol(SymbolToken::RIGHT_PARENTHESIS);

    iterator->next();

    auto ifScope=std::make_shared<StmListScope>(
        iterator->lineNumber,
        L"لو",
        parentScope
    );

    parseEvenWithNoCurlyBracesInScope(ifScope);

    SharedStmListScope elseScope;

    if(iterator->currentMatch(KeywordToken::ELSE)){
        iterator->next();
        elseScope=std::make_shared<StmListScope>(
            iterator->lineNumber,
            L"وإلا",
            parentScope
        );
        parseEvenWithNoCurlyBracesInScope(elseScope);
    }

    return std::make_shared<IfStatement>(
        lineNumber,
        parentScope,
        conditionEx,
        ifScope,
        elseScope
    );
}

SharedIStatement StmListParser::parseWhileStatement(SharedStmListScope parentScope){
    
    if(!iterator->currentMatch(KeywordToken::WHILE))
        return nullptr;

    int lineNumber=iterator->lineNumber;

    expectNextSymbol(SymbolToken::LEFT_PARENTHESIS);
    
    iterator->next();

    auto conditionEx=expressionParserProvider(iterator,parentScope)->parse();

    if(!conditionEx)
        throw ExpressionExpectedException(iterator->lineNumber);
    
    expectSymbol(SymbolToken::RIGHT_PARENTHESIS);

    iterator->next();

    auto whileScope=std::make_shared<LoopScope>(
        iterator->lineNumber,
        parentScope
    );

    parseEvenWithNoCurlyBracesInScope(whileScope);

    return std::make_shared<WhileStatement>(
        lineNumber,
        parentScope,
        conditionEx,
        whileScope
    );
}

SharedIStatement StmListParser::parseDoWhileStatement(SharedStmListScope parentScope){
    
    if(!iterator->currentMatch(KeywordToken::DO))
        return nullptr;

    int lineNumber=iterator->lineNumber;

    iterator->next();

    auto doWhileScope=std::make_shared<LoopScope>(
        iterator->lineNumber,
        parentScope
    );

    parseEvenWithNoCurlyBracesInScope(doWhileScope);

    if(!iterator->currentMatch(KeywordToken::WHILE))
        throw UnexpectedTokenException(
            lineNumber,
            LexerToken::stringify(LexerToken::KEYWORD_TOKEN)+L" "+KeywordToken::WHILE.getVal(),
            LexerToken::stringify(iterator->currentTokenType())+L" "+iterator->currentVal()
        );

    expectNextSymbol(SymbolToken::LEFT_PARENTHESIS);
    
    iterator->next();

    auto conditionEx=expressionParserProvider(iterator,parentScope)->parse();

    if(!conditionEx)
        throw ExpressionExpectedException(iterator->lineNumber);
    
    expectSymbol(SymbolToken::RIGHT_PARENTHESIS);

    iterator->next();
    
    return std::make_shared<DoWhileStatement>(
        lineNumber,
        parentScope,
        conditionEx,
        doWhileScope
    );

}

SharedIStatement StmListParser::parseReturnStatement(SharedStmListScope parentScope){
    
    if(!iterator->currentMatch(KeywordToken::RETURN))
        return nullptr;
    
    int lineNumber=iterator->lineNumber;

    iterator->next();

    auto ex=expressionParserProvider(iterator,parentScope)->parse();

    if(!ex)
        ex=std::make_shared<VoidExpression>(lineNumber);

    return std::make_shared<ReturnStatement>(
        lineNumber,
        parentScope,
        ex
    );
}

SharedIStatement StmListParser::parseBreakContinueStatement(SharedStmListScope parentScope){

    auto isBreak=iterator->currentMatch(KeywordToken::BREAK);
    auto isContinue=iterator->currentMatch(KeywordToken::CONTINUE);

    if(!isBreak&&!isContinue)
        return nullptr;

    auto loopScope=BaseScope::getContainingLoop(parentScope);
    
    int lineNumber=iterator->lineNumber;

    if(!loopScope)
        throw StmShouldBeUsedInsideLoopException(
            lineNumber,
            ((isBreak)?KeywordToken::BREAK.getVal():KeywordToken::CONTINUE.getVal())
        );
    
    iterator->next();

    if(isBreak)
        return std::make_shared<BreakStatement>(lineNumber,parentScope);

    return std::make_shared<ContinueStatement>(lineNumber,parentScope);
}

SharedIStatement StmListParser::parseExpressionStatement(SharedStmListScope parentScope){

    auto ex=expressionParserProvider(iterator,parentScope)->parse();

    if(!ex)
        return nullptr;

    if(auto exOfGet=IExpression::isGetOpFunInvokeExpression(ex))
        return parseAsSetOperatorExStm(parentScope,exOfGet);

    return parseAsAssignStm(parentScope,ex);
}

SharedIStatement StmListParser::parseAsSetOperatorExStm(
    SharedStmListScope parentScope,
    SharedOpFunInvokeExpression exOfGet
){

    auto op=iterator->currentToken();

    auto augOp=SetOperatorExpression::Operator::BAD_OP;

    if(*op==SymbolToken::PLUS_EQUAL)
        augOp=SetOperatorExpression::Operator::PLUS_EQUAL;
    else if(*op==SymbolToken::MINUS_EQUAL)
        augOp=SetOperatorExpression::Operator::MINUS_EQUAL;
    else if(*op==SymbolToken::STAR_EQUAL)
        augOp=SetOperatorExpression::Operator::TIMES_EQUAL;
    else if(*op==SymbolToken::SLASH_EQUAL)
        augOp=SetOperatorExpression::Operator::DIV_EQUAL;
    else if(*op==SymbolToken::MODULO_EQUAL)
        augOp=SetOperatorExpression::Operator::MOD_EQUAL;
    else if(*op==SymbolToken::POWER_EQUAL)
        augOp=SetOperatorExpression::Operator::POW_EQUAL;
    else if(*op==SymbolToken::SHR_EQUAL)
        augOp=SetOperatorExpression::Operator::SHR_EQUAL;
    else if(*op==SymbolToken::SHL_EQUAL)
        augOp=SetOperatorExpression::Operator::SHL_EQUAL;
    else if(*op==SymbolToken::BIT_AND_EQUAL)
        augOp=SetOperatorExpression::Operator::BIT_AND_EQUAL;
    else if(*op==SymbolToken::XOR_EQUAL)
        augOp=SetOperatorExpression::Operator::XOR_EQUAL;
    else if(*op==SymbolToken::BIT_OR_EQUAL)
        augOp=SetOperatorExpression::Operator::BIT_OR_EQUAL;
    else if(*op!=SymbolToken::EQUAL)
        return std::make_shared<ExpressionStatement>(
            exOfGet->getLineNumber(),
            parentScope,
            exOfGet
        );
    
    auto lineNumber=iterator->lineNumber;
    
    iterator->next();

    auto valueToSetEx=expressionParserProvider(iterator,parentScope)->parse();
        
    if(!valueToSetEx)
        throw ExpressionExpectedException(iterator->lineNumber);

    SharedIExpression exOfSet;

    switch(augOp){

        case SetOperatorExpression::Operator::PLUS_EQUAL:
        case SetOperatorExpression::Operator::MINUS_EQUAL:
        case SetOperatorExpression::Operator::TIMES_EQUAL:
        case SetOperatorExpression::Operator::DIV_EQUAL:
        case SetOperatorExpression::Operator::MOD_EQUAL:
        case SetOperatorExpression::Operator::POW_EQUAL:
        case SetOperatorExpression::Operator::SHR_EQUAL:
        case SetOperatorExpression::Operator::SHL_EQUAL:
        case SetOperatorExpression::Operator::BIT_AND_EQUAL:
        case SetOperatorExpression::Operator::XOR_EQUAL:
        case SetOperatorExpression::Operator::BIT_OR_EQUAL:{
            exOfSet=std::make_shared<SetOperatorExpression>(
                augOp,
                exOfGet,
                valueToSetEx
            );
            break;
        }
        default:{
            auto indexEx=exOfGet->getArgs()->at(0);
            auto args=std::vector<SharedIExpression>{
                indexEx,valueToSetEx
            };
            exOfSet=std::make_shared<OperatorFunInvokeExpression>(
                lineNumber,
                OperatorFunInvokeExpression::Operator::SET_EQUAL,
                std::make_shared<std::vector<SharedIExpression>>(args),
                exOfGet->getInside()
            );
        }
    }

    return std::make_shared<ExpressionStatement>(
        lineNumber,
        parentScope,
        exOfSet
    );
}

SharedIStatement StmListParser::parseAsAssignStm(
    SharedStmListScope parentScope,
    SharedIExpression leftEx
){
    auto op=iterator->currentToken();

    auto augOp=AugmentedAssignStatement::Operator::BAD_OP;

    if(*op==SymbolToken::PLUS_EQUAL)
        augOp=AugmentedAssignStatement::Operator::PLUS;
    else if(*op==SymbolToken::MINUS_EQUAL)
        augOp=AugmentedAssignStatement::Operator::MINUS;
    else if(*op==SymbolToken::STAR_EQUAL)
        augOp=AugmentedAssignStatement::Operator::TIMES;
    else if(*op==SymbolToken::SLASH_EQUAL)
        augOp=AugmentedAssignStatement::Operator::DIV;
    else if(*op==SymbolToken::MODULO_EQUAL)
        augOp=AugmentedAssignStatement::Operator::MOD;
    else if(*op==SymbolToken::POWER_EQUAL)
        augOp=AugmentedAssignStatement::Operator::POW;
    else if(*op==SymbolToken::SHR_EQUAL)
        augOp=AugmentedAssignStatement::Operator::SHR;
    else if(*op==SymbolToken::SHL_EQUAL)
        augOp=AugmentedAssignStatement::Operator::SHL;
    else if(*op==SymbolToken::BIT_AND_EQUAL)
        augOp=AugmentedAssignStatement::Operator::BIT_AND;
    else if(*op==SymbolToken::XOR_EQUAL)
        augOp=AugmentedAssignStatement::Operator::XOR;
    else if(*op==SymbolToken::BIT_OR_EQUAL)
        augOp=AugmentedAssignStatement::Operator::BIT_OR;
    else if(*op!=SymbolToken::EQUAL)
        return std::make_shared<ExpressionStatement>(
            leftEx->getLineNumber(),
            parentScope,
            leftEx
        );

    auto lineNumber=iterator->lineNumber;

    iterator->next();
    
    auto rightEx=expressionParserProvider(iterator,parentScope)->parse();

    if(!rightEx)
        throw ExpressionExpectedException(iterator->lineNumber);

    switch(augOp){
        case AugmentedAssignStatement::Operator::PLUS:
        case AugmentedAssignStatement::Operator::MINUS:
        case AugmentedAssignStatement::Operator::TIMES:
        case AugmentedAssignStatement::Operator::DIV:
        case AugmentedAssignStatement::Operator::MOD:
        case AugmentedAssignStatement::Operator::POW:
        case AugmentedAssignStatement::Operator::SHL:
        case AugmentedAssignStatement::Operator::SHR:
        case AugmentedAssignStatement::Operator::BIT_AND:
        case AugmentedAssignStatement::Operator::XOR:
        case AugmentedAssignStatement::Operator::BIT_OR:
            return std::make_shared<AugmentedAssignStatement>(
                lineNumber,
                parentScope,
                augOp,
                leftEx,
                rightEx
            );
        default:
            return std::make_shared<AssignStatement>(
                lineNumber,
                parentScope,
                leftEx,
                rightEx
            );
    }
}