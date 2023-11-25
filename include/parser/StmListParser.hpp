#pragma once
#include "BaseParser.hpp"
#include "ParserProvidersAliases.hpp"
#include "SharedPtrTypes.hpp"
#include "OperatorFunInvokeExpression.hpp"
#include <memory>
class StmListParser:public BaseParser<SharedStmList>{
    private:
        VarStmParserProvider varStmParserProvider;
        ExpressionParserProvider expressionParserProvider;
    public:
        StmListParser(
            SharedTokensIterator iterator,
            SharedStmListScope scope,
            VarStmParserProvider varStmParserProvider,
            ExpressionParserProvider expressionParserProvider
        );
        SharedStmList parse()override;
        void parseInScope(SharedStmListScope stmListScope);
        void parseEvenWithNoCurlyBracesInScope(SharedStmListScope stmListScope);
        SharedIStatement parseNextStatement(SharedStmListScope parentScope);
        SharedIStatement parseVarStatement(SharedStmListScope parentScope);
        SharedIStatement parseIfStatement(SharedStmListScope parentScope);
        SharedIStatement parseWhileStatement(SharedStmListScope parentScope);
        SharedIStatement parseDoWhileStatement(SharedStmListScope parentScope);
        SharedIStatement parseReturnStatement(SharedStmListScope parentScope);
        SharedIStatement parseBreakContinueStatement(SharedStmListScope parentScope);
        SharedIStatement parseExpressionStatement(SharedStmListScope parentScope);
        SharedIStatement parseAsSetOperatorExStm(
            SharedStmListScope parentScope,
            SharedOpFunInvokeExpression exOfGet
        );
        SharedIStatement parseAsAssignStm(
            SharedStmListScope parentScope,
            SharedIExpression leftEx
        );

};