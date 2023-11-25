#include "FileScope.hpp"
#include "FunctionNotFoundException.hpp"
#include "PackageScope.hpp"
#include "FunScope.hpp"
#include "ClassScope.hpp"
#include "SharedPtrTypes.hpp"
#include "Type.hpp"
#include "IStatement.hpp"
#include "Variable.hpp"
#include <memory>
#include <string>

FileScope::FileScope(std::wstring filePath,SharedPackageScope parentScope)
:BaseScope(0,filePath,parentScope),
publicFunctions(std::make_shared<std::unordered_map<std::wstring,SharedFunScope>>()),
privateFunctions(std::make_shared<std::unordered_map<std::wstring,SharedFunScope>>()),
publicClasses(std::make_shared<std::unordered_map<std::wstring,SharedClassScope>>()),
privateClasses(std::make_shared<std::unordered_map<std::wstring,SharedClassScope>>()),
publicVariables(std::make_shared<std::unordered_map<std::wstring,SharedVariable>>()),
privateVariables(std::make_shared<std::unordered_map<std::wstring,SharedVariable>>())
{}

SharedMap<std::wstring,SharedFunScope> FileScope::getPublicFunctions()const{
    return publicFunctions;
}

SharedMap<std::wstring,SharedFunScope> FileScope::getPrivateFunctions()const{
    return privateFunctions;
}

SharedClassScope FileScope::getClassByType(SharedType type){
    auto classIterator=privateClasses->find(*type->getName());

    if(classIterator!=privateClasses->end())
        return classIterator->second;
    
    return parentScope->getClassByType(type);
}

SharedMap<std::wstring,SharedClassScope> FileScope::getPublicClasses()const{
    return publicClasses;
}

SharedMap<std::wstring,SharedClassScope> FileScope::getPrivateClasses()const{
    return privateClasses;
}

SharedMap<std::wstring,SharedVariable> FileScope::getPublicVariables()const{
    return this->publicVariables;
}

SharedMap<std::wstring,SharedVariable> FileScope::getPrivateVariables()const{
    return this->privateVariables;
}

SharedFunScope FileScope::findPublicFunction(std::wstring decl){
    auto funIterator=publicFunctions->find(decl);

    if(funIterator!=publicFunctions->end())
        return funIterator->second;
    
    return nullptr;
}

SharedFunScope FileScope::findPrivateFunction(std::wstring decl){
    auto funIterator=privateFunctions->find(decl);

    if(funIterator!=privateFunctions->end())
        return funIterator->second;
        
    return nullptr;
}

SharedClassScope FileScope::findPublicClass(std::wstring name){
    auto classIterator=publicClasses->find(name);

    if(classIterator!=publicClasses->end())
        return classIterator->second;
        
    return nullptr;
}

SharedClassScope FileScope::findPrivateClass(std::wstring name){
    auto classIterator=privateClasses->find(name);

    if(classIterator!=privateClasses->end())
        return classIterator->second;
        
    return nullptr;
}


SharedVariable FileScope::findPublicVariable(std::wstring varName){
    auto varIterator=publicVariables->find(varName);

    if(varIterator!=publicVariables->end())
        return varIterator->second;
    
    return nullptr;
}

SharedVariable FileScope::findPrivateVariable(std::wstring varName){
    auto varIterator=privateVariables->find(varName);

    if(varIterator!=privateVariables->end())
        return varIterator->second;
    
    return nullptr;
}

SharedStmListScope FileScope::getGlobalVarsInitStmList()const{
    return this->globalVarsInitStmList;
}

void FileScope::setGlobalVarsInitStmList(SharedStmListScope globalVarsInitStmList){
    this->globalVarsInitStmList=globalVarsInitStmList;
}