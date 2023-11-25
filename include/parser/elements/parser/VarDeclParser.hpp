#pragma once
#include "BaseParser.hpp"
#include "ParserProvidersAliases.hpp"
#include "SharedPtrTypes.hpp"
class VarDeclParser:public BaseParser<SharedVarDecl>{
    private:
        SharedBaseParser<SharedType> typeParser;
    public:
        VarDeclParser(
            SharedTokensIterator iterator,
            SharedBaseScope scope,
            TypeParserProvider typeParserProvider
        );
        SharedVarDecl parse()override;
};