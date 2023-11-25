#include "IStatement.hpp"

IStatement::IStatement(int lineNumber,SharedBaseScope runScope)
:ASTNode(lineNumber),runScope(runScope){}

SharedBaseScope IStatement::getRunScope()const{
    return runScope;
}
