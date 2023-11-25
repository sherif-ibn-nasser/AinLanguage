#include "FunParser.hpp"
#include "FunDeclParser.hpp"
#include "SharedPtrTypes.hpp"
#include "SymbolToken.hpp"
#include "Type.hpp"
#include "TypeChecker.hpp"
#include "ClassScope.hpp"
#include "FunDecl.hpp"
#include "FunScope.hpp"
#include "ClassScope.hpp"
#include "FunParam.hpp"
#include "Variable.hpp"
#include "FunScope.hpp"
#include <map>
#include <memory>
#include <string>
#include <vector>

FunParser::FunParser(
    SharedTokensIterator iterator,
    SharedBaseScope scope,
    FunDeclParserProvider funDeclParserProvider,
    StmListParserProvider stmListParserProvider
):BaseParser(iterator,scope),
funDeclParser(
    funDeclParserProvider(iterator,scope)
),
stmListParserProvider(stmListParserProvider){}

SharedFunScope FunParser::parse(){

    auto lineNumber=iterator->lineNumber;

    auto decl=funDeclParser->parse();

    if(!decl)
        return nullptr;
    
    if(decl->hasImplicitReturnType())
        decl->returnType=Type::UNIT;
    
    auto funScope=std::make_shared<FunScope>(
        lineNumber,
        scope,
        decl
    );
    
    auto locals=funScope->getLocals();

    for(auto param:*decl->params){

        auto paramVal=std::make_shared<Variable>(
            param->name,
            param->type,
            std::make_shared<bool>(true)
        );
        
        (*locals)[*param->name]=paramVal;

    }

    auto stmListParser=stmListParserProvider(iterator,funScope);

    stmListParser->parse();

    return funScope;
    
}