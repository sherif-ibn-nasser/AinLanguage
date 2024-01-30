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
#include <memory>
#include <string>
#include <unordered_map>
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

SharedMap<std::wstring, SharedVariable> FunScope::getParamsFromLocals(){
    auto params=std::make_shared<std::unordered_map<std::wstring, SharedVariable>>();

    for (auto param:*decl->params){
        (*params)[*param->name]=(*locals)[*param->name];
    }

    return params;
}

SharedMap<std::wstring, SharedVariable> FunScope::getNonParamsFromLocals(){
    auto nonParams=std::make_shared<std::unordered_map<std::wstring, SharedVariable>>();
    auto params=getParamsFromLocals();

    for (auto localIt:*locals){
        auto name=localIt.first;
        if(params->find(name)!=params->end())
            continue;
        (*nonParams)[name]=(*locals)[name];
    }

    return nonParams;
}