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

    auto isVal=iterator->currentMatch(KeywordToken::VAL);

    if(
        !iterator->currentMatch(KeywordToken::VAR)
        &&
        !isVal
    )
        return nullptr;

    auto nameId=expectNextIdentifier();

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
        std::make_shared<bool>(isVal)
    );

    return var;
}
