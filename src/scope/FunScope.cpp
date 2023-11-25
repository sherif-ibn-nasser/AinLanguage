#include "FunScope.hpp"
#include "ClassScope.hpp"
#include "FunDecl.hpp"
#include "OperatorFunctions.hpp"
#include "SharedPtrTypes.hpp"
#include "FunParam.hpp"
#include "StmListScope.hpp"
#include "Type.hpp"
#include "Variable.hpp"
#include "IStatement.hpp"
#include "FunDecl.hpp"
#include <string>
#include "InvalidOperatorFunDeclarationException.hpp"

FunScope::FunScope(
    int lineNumber,
    SharedBaseScope parentScope,
    SharedFunDecl decl
):StmListScope(lineNumber,*decl->name,parentScope),decl(decl)
{}

SharedType FunScope::getReturnType(){
    return this->decl->returnType;
}

SharedFunDecl FunScope::getDecl(){
    return this->decl;
}

void FunScope::setReturnValue(SharedIValue returnValue){
    this->returnValue=returnValue;
}

SharedIValue FunScope::getReturnValue(){
    return this->returnValue;
}