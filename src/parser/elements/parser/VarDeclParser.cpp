#include "VarDeclParser.hpp"
#include "VarDecl.hpp"
#include "KeywordToken.hpp"
#include "SharedPtrTypes.hpp"
#include "TypeParser.hpp"
#include "Type.hpp"
#include <memory>

VarDeclParser::VarDeclParser(
    SharedTokensIterator iterator,
    SharedBaseScope scope,
    TypeParserProvider typeParserProvider
):BaseParser(iterator,scope),
typeParser(
    typeParserProvider(iterator,scope)
){}

SharedVarDecl VarDeclParser::parse(){

    if(!iterator->currentMatch(KeywordToken::LET))
        return nullptr;

    auto isVar=iterator->nextMatch(KeywordToken::MUT);

    if(isVar)
        iterator->next();

    auto nameId=expectIdentifier();

    auto name=std::make_shared<std::wstring>(nameId);

    SharedType type;

    auto colonFound=false;
    
    try{
        expectNextSymbol(SymbolToken::COLON);
        colonFound=true;
    }catch(UnexpectedTokenException& e){}

    if(colonFound){
        iterator->next();
        type=typeParser->parse();
    }

    auto var=std::make_shared<VarDecl>(
        name,
        type,
        std::make_shared<bool>(!isVar) // isVal
    );

    return var;
}
