#pragma once
#include "BaseParser.hpp"
#include "ParserProvidersAliases.hpp"
#include "SharedPtrTypes.hpp"

class FunDeclParser:public BaseParser<SharedFunDecl>{
    private:
        SharedBaseParser<SharedType> returnTypeParser;
        SharedBaseParser<SharedFunParam> paramsParser;
    public:
        FunDeclParser(
            SharedTokensIterator iterator,
            SharedBaseScope scope,
            TypeParserProvider typeParserProvider,
            FunParamParserProvider paramsParserProvider
        );
        SharedFunDecl parse()override;
};