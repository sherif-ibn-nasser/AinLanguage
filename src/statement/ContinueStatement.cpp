#include "ContinueStatement.hpp"
#include "LoopScope.hpp"

ContinueStatement::ContinueStatement(
    int lineNumber,
    SharedBaseScope runScope
):IStatement(lineNumber,runScope){}