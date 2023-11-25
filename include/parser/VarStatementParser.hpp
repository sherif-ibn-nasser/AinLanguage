#pragma once
#include "BaseParser.hpp"
#include "ParserProvidersAliases.hpp"
#include "SharedPtrTypes.hpp"
#include <memory>
class VarStatementParser:public BaseParser<SharedVarStm>{
    private:
        SharedBaseParser<SharedVarDecl> varDeclParser;
        SharedBaseParser<SharedIExpression> expressionParser;
    public:
        VarStatementParser(
            SharedTokensIterator iterator,
            SharedBaseScope scope,
            VarDeclParserProvider varDeclParserProvider,
            ExpressionParserProvider expressionParserProvider
        );
        SharedVarStm parse()override;
};