#include "FunParamParser.hpp"
#include "FunParam.hpp"
#include "TypeParser.hpp"
#include "SharedPtrTypes.hpp"

FunParamParser::FunParamParser(
    SharedTokensIterator iterator,
    SharedBaseScope scope,
    TypeParserProvider typeParserProvider
):
BaseParser(iterator,scope),
typeParser(
    typeParserProvider(iterator,scope)
){}

SharedFunParam FunParamParser::parse(){

    auto nameId=expectIdentifier();

    expectNextSymbol(SymbolToken::COLON);

    iterator->next();

    auto paramName=std::make_shared<std::wstring>(nameId);

    auto paramType=typeParser->parse();

    auto param=std::make_shared<FunParam>(paramName,paramType);

    return param;
}
