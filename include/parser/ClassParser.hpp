#pragma once
#include "BaseParser.hpp"
#include "ParserProvidersAliases.hpp"
#include "SharedPtrTypes.hpp"
class ClassParser:public BaseParser<SharedClassScope>{
    private:
        StmListParserProvider stmListParserProvider;
        TypeParserProvider typeParserProvider;
        FunParamParserProvider paramsParserProvider;
        FunParserProvider funParserProvider;
        VarStmParserProvider varStmParserProvider;
        enum class VisibilityModifier{
            NOT_PARSED,PUBLIC,PRIVATE
        };
        VisibilityModifier visibility;
        void resetVisibility();
        void parseVisibility();
        void parseConstructor(SharedType parentType);
        void parseFunScope(SharedType parentType);
        void parseVarStm(SharedType parentType);
    public:
        ClassParser(
            SharedTokensIterator iterator,
            SharedBaseScope scope,
            StmListParserProvider stmListParserProvider,
            TypeParserProvider typeParserProvider,
            FunParamParserProvider paramsParserProvider,
            FunParserProvider funParserProvider,
            VarStmParserProvider varStmParserProvider
        );
        SharedClassScope parse()override;
};