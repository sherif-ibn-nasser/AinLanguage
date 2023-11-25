#include "FunDecl.hpp"
#include "FunParam.hpp"
#include "KeywordToken.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"
#include <memory>
#include <string>

FunDecl::FunDecl(
    SharedWString name,
    SharedType returnType,
    SharedBool isOperator,
    SharedVector<SharedFunParam> params
):name(name),returnType(returnType),isOperator(isOperator),params(params){}

bool FunDecl::operator==(const FunDecl& declaration)const{

    if(params->size()!=declaration.params->size())
        return false;

    for(int i=0;i<params->size();i++){
        if(*params->at(i)!=*(declaration.params->at(i)))
            return false;
    }
    
    if(!(returnType&&declaration.returnType))
        return false;

    return
        *name==*declaration.name
        &&
        *returnType==*declaration.returnType
    ;
}

bool FunDecl::operator!=(const FunDecl& declaration)const{
    return !(*this==declaration);
}

bool FunDecl::hasImplicitReturnType()const {
    return this->returnType==nullptr;
}

bool FunDecl::isConstructor(){
    return *this->name==KeywordToken::NEW.getVal();
}

std::wstring FunDecl::toString(){
    std::wstring paramsTypes=L"";
    int i=0;
    for(auto param:*params){
        paramsTypes.append(*param->type->getName());

        if(i!=params->size()-1)
            paramsTypes.append(L" ،");

        i++;
    }
    return *this->name+L"("+paramsTypes+L")";
}