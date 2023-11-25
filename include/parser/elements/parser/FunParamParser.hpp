#pragma once
#include "BaseParser.hpp"
#include "ParserProvidersAliases.hpp"
#include "SharedPtrTypes.hpp"
class FunParamParser:public BaseParser<SharedFunParam>{
    private:
        SharedBaseParser<SharedType> typeParser;
    public:
        FunParamParser(
            SharedTokensIterator iterator,
            SharedBaseScope scope,
            TypeParserProvider typeParserProvider
        );
        SharedFunParam parse()override;
};