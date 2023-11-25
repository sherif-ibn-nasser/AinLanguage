#include "VarStatementParser.hpp"
#include "ExpressionExpectedException.hpp"
#include "MustHaveExplicitTypeException.hpp"
#include "ParserProvidersAliases.hpp"
#include "SharedPtrTypes.hpp"
#include "SymbolToken.hpp"
#include "VarDecl.hpp"
#include "VarStm.hpp"
#include "Variable.hpp"
#include "ImplicitVarTypeChecker.hpp"
#include <memory>

VarStatementParser::VarStatementParser(
    SharedTokensIterator iterator,
    SharedBaseScope scope,
    VarDeclParserProvider varDeclParserProvider,
    ExpressionParserProvider expressionParserProvider
):
BaseParser(iterator,scope),
varDeclParser(
    varDeclParserProvider(iterator,scope)
),
expressionParser(
    expressionParserProvider(iterator,scope)
)
{}

SharedVarStm VarStatementParser::parse(){
    auto decl=varDeclParser->parse();

    if(!decl)
        return nullptr;
        
    SharedIExpression ex;

    int lineNumber=iterator->lineNumber;

    if(iterator->currentMatch(SymbolToken::EQUAL)){
        iterator->next();
        ex=expressionParser->parse();
        if(!ex)
            throw ExpressionExpectedException(iterator->lineNumber);
    }
    else if(decl->hasImplicitType())
        throw MustHaveExplicitTypeException(lineNumber);
    
    auto var=std::make_shared<Variable>(decl);

    auto stm=std::make_shared<VarStm>(
        lineNumber,
        scope,
        var,
        ex
    );

    if(decl->hasImplicitType()){
        Semantics::ImplicitVarTypeChecker::getInstance()->subscribe(stm);
    }

    return stm;
}
