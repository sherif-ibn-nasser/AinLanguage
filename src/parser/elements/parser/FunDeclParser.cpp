#include "FunDeclParser.hpp"
#include "ConflictingDeclarationException.hpp"
#include "FunDecl.hpp"
#include "KeywordToken.hpp"
#include "LexerToken.hpp"
#include "InvalidOperatorFunDeclarationException.hpp"
#include "OperatorFunctions.hpp"
#include "ParserProvidersAliases.hpp"
#include "SharedPtrTypes.hpp"
#include "SymbolToken.hpp"
#include "TokensIterator.hpp"
#include "Type.hpp"
#include "TypeParser.hpp"
#include "UnexpectedTokenException.hpp"
#include "FunParamParser.hpp"
#include "FunParam.hpp"
#include <algorithm>
#include <functional>
#include <memory>
#include <string>
#include <vector>

FunDeclParser::FunDeclParser(
    SharedTokensIterator iterator,
    SharedBaseScope scope,
    TypeParserProvider typeParserProvider,
    FunParamParserProvider paramsParserProvider
):
BaseParser(iterator,scope),
returnTypeParser(
    typeParserProvider(iterator,scope)
),
paramsParser(
    paramsParserProvider(iterator,scope,typeParserProvider)
){}

SharedFunDecl FunDeclParser::parse(){

    if(!iterator->currentMatch(KeywordToken::FUN))
        return nullptr;
    
    auto isOperator=std::make_shared<bool>(false);

    if(iterator->nextMatch(KeywordToken::OPERATOR)){
        *isOperator=true;
        iterator->next();
    }

    auto funNameId=expectIdentifier();

    int lineNumber=iterator->lineNumber;

    if(*isOperator&&!OperatorFunctions::isOperatorFunName(funNameId))
        throw InvalidOperatorFunDeclarationException(L"اسم الدالة غير صالح");

    auto funName=std::make_shared<std::wstring>(funNameId);

    expectNextSymbol(SymbolToken::LEFT_PARENTHESIS);

    auto params=std::make_shared<std::vector<SharedFunParam>>();

    do{

        if(iterator->nextMatch(SymbolToken::RIGHT_PARENTHESIS))
            break;

        auto param=paramsParser->parse();

        auto isParamDeclared=std::find_if(
            params->begin(),
            params->end(),
            [&](SharedFunParam p){return *p->name==*param->name;}
        );

        if(isParamDeclared!=params->end())
            throw ConflictingDeclarationException(iterator->lineNumber);

        params->push_back(param);
        
    }while(iterator->currentMatch(SymbolToken::COMMA));

    expectSymbol(SymbolToken::RIGHT_PARENTHESIS);

    SharedType funReturnType;

    if(iterator->nextMatch(SymbolToken::COLON)){
        iterator->next();
        funReturnType=returnTypeParser->parse();
    }

    return std::make_shared<FunDecl>(
        funName,
        funReturnType,
        isOperator,
        params
    );

}
