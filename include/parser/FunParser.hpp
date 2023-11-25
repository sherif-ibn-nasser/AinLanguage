#pragma once
#include "BaseParser.hpp"
#include "ParserProvidersAliases.hpp"
#include "SharedPtrTypes.hpp"

class FunParser:public BaseParser<SharedFunScope>{
    private:
        SharedBaseParser<SharedFunDecl> funDeclParser;
        StmListParserProvider stmListParserProvider;
    public:
        FunParser(
            SharedTokensIterator iterator,
            SharedBaseScope scope,
            FunDeclParserProvider funDeclParserProvider,
            StmListParserProvider stmListParserProvider
        );
        SharedFunScope parse()override;
};