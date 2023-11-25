#include "LoopScope.hpp"
#include "FunScope.hpp"
#include "IStatement.hpp"
#include "SharedPtrTypes.hpp"

LoopScope::LoopScope(int lineNumber,SharedBaseScope parentScope):
    StmListScope(lineNumber,L"حلقة_تكرارية",parentScope){}