#pragma once
#include "SharedPtrTypes.hpp"
#include <functional>
#include <memory>
#include <string>

using TypeParserProvider=std::function<SharedBaseParser<SharedType>(
    SharedTokensIterator iterator,
    SharedBaseScope scope
)>;

using FunParamParserProvider=std::function<SharedBaseParser<SharedFunParam>(
    SharedTokensIterator iterator,
    SharedBaseScope scope,
    TypeParserProvider typeParserProvider
)>;

using FunDeclParserProvider=std::function<SharedBaseParser<SharedFunDecl>(
    SharedTokensIterator iterator,
    SharedBaseScope scope
)>;

using StmListParserProvider=std::function<SharedBaseParser<SharedStmList>(
    SharedTokensIterator iterator,
    SharedStmListScope scope
)>;

using VarDeclParserProvider=std::function<SharedBaseParser<SharedVarDecl>(
    SharedTokensIterator iterator,
    SharedBaseScope scope
)>;

using ExpressionParserProvider=std::function<SharedBaseParser<SharedIExpression>(
    SharedTokensIterator iterator,
    SharedBaseScope scope
)>;

using VarStmParserProvider=std::function<SharedBaseParser<SharedVarStm>(
    SharedTokensIterator iterator,
    SharedBaseScope scope
)>;

using FunParserProvider=std::function<SharedBaseParser<SharedFunScope>(
    SharedTokensIterator iterator,
    SharedBaseScope scope
)>;

using ClassParserProvider=std::function<SharedBaseParser<SharedClassScope>(
    SharedTokensIterator iterator,
    SharedBaseScope scope
)>;