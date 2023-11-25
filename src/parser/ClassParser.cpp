#include "ClassParser.hpp"
#include "BuiltInFunScope.hpp"
#include "ClassScope.hpp"
#include "ConflictingDeclarationException.hpp"
#include "FunDecl.hpp"
#include "FunParam.hpp"
#include "FunScope.hpp"
#include "KeywordToken.hpp"
#include "LexerToken.hpp"
#include "SharedPtrTypes.hpp"
#include "SymbolToken.hpp"
#include "Type.hpp"
#include "ainio.hpp"
#include "VarStm.hpp"
#include "Variable.hpp"
#include <memory>
#include <string>

ClassParser::ClassParser(
    SharedTokensIterator iterator,
    SharedBaseScope scope,
    StmListParserProvider stmListParserProvider,
    TypeParserProvider typeParserProvider,
    FunParamParserProvider paramsParserProvider,
    FunParserProvider funParserProvider,
    VarStmParserProvider varStmParserProvider
):
    BaseParser(iterator,scope),
    stmListParserProvider(stmListParserProvider),
    typeParserProvider(typeParserProvider),
    paramsParserProvider(paramsParserProvider),
    funParserProvider(funParserProvider),
    varStmParserProvider(varStmParserProvider){}

SharedClassScope ClassParser::parse(){

    if(!iterator->currentMatch(KeywordToken::CLASS))
        return nullptr;

    auto lineNumber=iterator->lineNumber;

    auto className=expectNextIdentifier();

    iterator->next();

    expectSymbol(SymbolToken::LEFT_CURLY_BRACES);
    
    iterator->next();
    
    auto classScope=std::make_shared<ClassScope>(
        lineNumber,
        className,
        scope
    );

    classScope->setVarsInitStmList(
        std::make_shared<StmListScope>(
            lineNumber,
            className,
            classScope
        )
    );

    auto type=std::make_shared<Type>(
        std::make_shared<std::wstring>(className),
        classScope
    );

    resetVisibility();
    
    while(iterator->currentTokenType()!=LexerToken::EOF_TOKEN){

        if(iterator->currentMatch(SymbolToken::RIGHT_CURLY_BRACES))
            break;

        parseConstructor(type);

        parseFunScope(type);
        
        parseVarStm(type);

    }
    
    expectSymbol(SymbolToken::RIGHT_CURLY_BRACES);

    iterator->next();

    if(classScope->getPublicConstructors()->empty()&&classScope->getPrivateConstructors()->empty()){
        // Add default constructor
        auto decl=std::make_shared<FunDecl>(
            std::make_shared<std::wstring>(KeywordToken::NEW.getVal()),
            type,
            std::make_shared<bool>(false),
            std::make_shared<std::vector<SharedFunParam>>()
        );

        (*classScope->getPublicConstructors())[decl->toString()]=
            std::make_shared<FunScope>(lineNumber,classScope,decl);
    }

    return classScope;
}

void ClassParser::resetVisibility(){
    visibility=VisibilityModifier::NOT_PARSED;
}

void ClassParser::parseVisibility(){

    if(visibility!=VisibilityModifier::NOT_PARSED)
        return;
    
    visibility=VisibilityModifier::PUBLIC;

    if(iterator->currentMatch(KeywordToken::PUBLIC)){
        iterator->next();
    }

    else if(iterator->currentMatch(KeywordToken::PRIVATE)){
        visibility=VisibilityModifier::PRIVATE;
        iterator->next();
    }

}

void ClassParser::parseConstructor(SharedType parentType){
    parseVisibility();

    if(!iterator->currentMatch(KeywordToken::NEW))
        return;
    
    auto parentScope=parentType->getClassScope();
    
    auto lineNumber=iterator->lineNumber;

    expectNextSymbol(SymbolToken::LEFT_PARENTHESIS);

    auto params=std::make_shared<std::vector<SharedFunParam>>();

    auto paramsParser=paramsParserProvider(iterator,parentScope,typeParserProvider);

    do{

        if(iterator->nextMatch(SymbolToken::RIGHT_PARENTHESIS))
            break;

        auto param=paramsParser->parse();

        auto isParamDeclared=std::find_if(
            params->begin(),
            params->end(),
            [&](SharedFunParam p){return *p->name==*param->name;}
        );

        if(isParamDeclared!=params->end())
            throw ConflictingDeclarationException(iterator->lineNumber);

        params->push_back(param);
        
    }while(iterator->currentMatch(SymbolToken::COMMA));

    expectSymbol(SymbolToken::RIGHT_PARENTHESIS);

    iterator->next();

    auto decl=std::make_shared<FunDecl>(
        std::make_shared<std::wstring>(KeywordToken::NEW.getVal()),
        parentType,
        std::make_shared<bool>(false),
        params
    );

    auto declStr=decl->toString();
    
    auto constructorScope=std::make_shared<FunScope>(
        lineNumber,
        parentScope,
        decl
    );

    if(parentScope->findPublicConstructor(declStr)||parentScope->findPrivateConstructor(declStr))
        throw ConflictingDeclarationException(lineNumber);

    switch(visibility){
        case VisibilityModifier::PUBLIC:
            (*parentScope->getPublicConstructors())[declStr]=constructorScope;
            break;

        case VisibilityModifier::PRIVATE:
            (*parentScope->getPrivateConstructors())[declStr]=constructorScope;
            break;

        default:{}
    }

    resetVisibility();

    auto locals=constructorScope->getLocals();

    for(auto param:*decl->params){

        auto paramVal=std::make_shared<Variable>(
            param->name,
            param->type,
            std::make_shared<bool>(true)
        );
        
        (*locals)[*param->name]=paramVal;

    }

    auto stmListParser=stmListParserProvider(iterator,constructorScope);

    stmListParser->parse();
}

void ClassParser::parseFunScope(SharedType parentType){

    parseVisibility();

    auto lineNumber=iterator->lineNumber;

    auto parentScope=parentType->getClassScope();
    
    auto funScope=funParserProvider(iterator,parentScope)->parse();

    if(!funScope)
        return;
        
    auto decl=funScope->getDecl();
    
    auto declStr=decl->toString();

    if(parentScope->findPrivateFunction(declStr)||parentScope->findPublicFunction(declStr))
        throw ConflictingDeclarationException(lineNumber);
    
    switch(visibility){
        case VisibilityModifier::PUBLIC:
            (*parentScope->getPublicFunctions())[declStr]=funScope;
            break;

        case VisibilityModifier::PRIVATE:
            (*parentScope->getPrivateFunctions())[declStr]=funScope;
            break;

        default:{}
    }

    resetVisibility();
}

void ClassParser::parseVarStm(SharedType parentType){
    
    parseVisibility();

    auto lineNumber=iterator->lineNumber;

    auto parentScope=parentType->getClassScope();
   
    auto varStm=varStmParserProvider(iterator,parentScope)->parse();

    if(!varStm)
        return;
    
    auto var=varStm->getVar();

    auto varName=*var->getName();

    auto primaryConstructor=parentScope->getVarsInitStmList();

    primaryConstructor->getStmList()->push_back(varStm);

    if(parentScope->findPrivateVariable(varName)||parentScope->findPublicVariable(varName))
        throw ConflictingDeclarationException(lineNumber);
    
    switch (visibility){
        case VisibilityModifier::PUBLIC:
            (*parentScope->getPublicVariables())[varName]=var;
            break;

        case VisibilityModifier::PRIVATE:
            (*parentScope->getPrivateVariables())[varName]=var;
            break;

        default:{}
    }

    resetVisibility();
}