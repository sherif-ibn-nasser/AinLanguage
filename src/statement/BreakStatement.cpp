#include "BreakStatement.hpp"
#include "LoopScope.hpp"

BreakStatement::BreakStatement(
    int lineNumber,
    SharedBaseScope runScope
):IStatement(lineNumber,runScope){}