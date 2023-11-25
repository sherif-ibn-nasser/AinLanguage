#include "FileParser.hpp"
#include "ConflictingDeclarationException.hpp"
#include "InvalidConstructorDeclarationException.hpp"
#include "InvalidOperatorFunDeclarationException.hpp"
#include "KeywordToken.hpp"
#include "LexerToken.hpp"
#include "PackageParser.hpp"
#include "PackageScope.hpp"
#include "FileScope.hpp"
#include "ClassScope.hpp"
#include "FunScope.hpp"
#include "SharedPtrTypes.hpp"
#include "FunDecl.hpp"
#include "VarStm.hpp"
#include "Variable.hpp"
#include <memory>

FileParser::FileParser(
    SharedTokensIterator iterator,
    std::wstring filePath,
    SharedBaseParser<SharedPackageScope> packageParser,
    FunParserProvider funParserProvider,
    ClassParserProvider classParserProvider,
    VarStmParserProvider varStmParserProvider
)
:BaseParser(iterator,nullptr),
filePath(filePath),
packageParser(packageParser),
funParserProvider(funParserProvider),
classParserProvider(classParserProvider),
varStmParserProvider(varStmParserProvider)
{}

SharedFileScope FileParser::parse(){
    
    auto package=packageParser->parse();

    scope=package;

    auto file=std::make_shared<FileScope>(filePath,package);

    package->addFile(file);

    fileScope=file;

    packageScope=package;

    fileScope->setGlobalVarsInitStmList(
        std::make_shared<StmListScope>(
            0,filePath,fileScope
        )
    );

    resetVisibility();

    while(iterator->currentTokenType()!=LexerToken::EOF_TOKEN){
        parseFunScope();
        parseClassScope();
        parseVarStm();
    }

    return file;
}

void FileParser::resetVisibility(){
    visibility=VisibilityModifier::NOT_PARSED;
}

void FileParser::parseVisibility(){

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

void FileParser::parseFunScope(){

    parseVisibility();

    auto lineNumber=iterator->lineNumber;
    
    auto funScope=funParserProvider(iterator,fileScope)->parse();

    if(!funScope)
        return;

    if(*funScope->getDecl()->isOperator)
        throw InvalidOperatorFunDeclarationException(
            L"يجب أن تكون الدالة داخل تصنيف."
        );

    if(funScope->getDecl()->isConstructor())
        throw InvalidConstructorDeclarationException(
            L"يجب أن تكون الدالة داخل تصنيف."
        );

    auto decl=funScope->getDecl()->toString();

    if(fileScope->findPrivateFunction(decl))
        throw ConflictingDeclarationException(lineNumber);

    switch(visibility){
        case VisibilityModifier::PUBLIC:{
            auto files=packageScope->getFiles();
            for(auto fileIt:files){
                if(fileIt.second->findPublicFunction(decl))
                    throw ConflictingDeclarationException(lineNumber);
            }
            (*fileScope->getPublicFunctions())[decl]=funScope;
            break;
        }

        case VisibilityModifier::PRIVATE:{
            if(fileScope->findPublicFunction(decl))
                throw ConflictingDeclarationException(lineNumber);
            
            (*fileScope->getPrivateFunctions())[decl]=funScope;
            break;
        }

        default:{}
    }

    resetVisibility();
}

void FileParser::parseClassScope(){

    parseVisibility();

    auto lineNumber=iterator->lineNumber;

    auto classScope=classParserProvider(iterator,fileScope)->parse();
        
    if(!classScope)
        return;
    
    auto className=classScope->getName();

    if(fileScope->findPrivateClass(className))
        throw ConflictingDeclarationException(lineNumber);
    
    switch(visibility){
        case VisibilityModifier::PUBLIC:{
            auto files=packageScope->getFiles();
            for(auto fileIt:files){
                if(fileIt.second->findPublicClass(className))
                    throw ConflictingDeclarationException(lineNumber);
            }
            (*fileScope->getPublicClasses())[className]=classScope;
            break;
        }

        case VisibilityModifier::PRIVATE:{
            if(fileScope->findPublicClass(className))
                throw ConflictingDeclarationException(lineNumber);
            
            (*fileScope->getPrivateClasses())[className]=classScope;
            break;
        }

        default:{}
    }

    resetVisibility();
}

void FileParser::parseVarStm(){

    parseVisibility();

    auto lineNumber=iterator->lineNumber; 
   
    auto varStm=varStmParserProvider(iterator,fileScope)->parse();

    if(!varStm)
        return;
    
    auto var=varStm->getVar();

    auto varName=*var->getName();

    auto globalVarsInitStmList=fileScope->getGlobalVarsInitStmList();

    globalVarsInitStmList->getStmList()->push_back(varStm);

    if(fileScope->findPrivateVariable(varName)||fileScope->findPublicVariable(varName))
        throw ConflictingDeclarationException(lineNumber);
    
    switch(visibility){
        case VisibilityModifier::PUBLIC:{
            auto files=packageScope->getFiles();
            for(auto fileIt:files){
                if(fileIt.second->findPublicVariable(varName))
                    throw ConflictingDeclarationException(lineNumber);
            }
            (*fileScope->getPublicVariables())[varName]=var;
            break;
        }

        case VisibilityModifier::PRIVATE:{
            if(fileScope->findPublicClass(varName))
                throw ConflictingDeclarationException(lineNumber);

            (*fileScope->getPrivateVariables())[varName]=var;
            break;
        }
        
        default:{}
    }

    resetVisibility();
}

std::wstring FileParser::getFilePath(){
    return filePath;
}