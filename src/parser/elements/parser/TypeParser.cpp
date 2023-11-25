#include "TypeParser.hpp"
#include "SharedPtrTypes.hpp"
#include "SymbolToken.hpp"
#include "Type.hpp"
#include "TokensIterator.hpp"
#include "TypeChecker.hpp"
#include "FileScope.hpp"
#include "BaseScope.hpp"
#include <memory>
#include <string>

TypeParser::TypeParser(
    SharedTokensIterator iterator,
    SharedBaseScope scope,
    SharedITypeChecker typeChecker
):BaseParser(iterator,scope),typeChecker(typeChecker){}

SharedType TypeParser::parse(){
    // TODO: Parse generics and nullables

    // ANCHOR: Do we need to specify maximum dimension of an array?

    if(iterator->currentMatch(SymbolToken::LEFT_SQUARE_BRACKET)){
        iterator->next();
        auto type=parse();
        expectSymbol(SymbolToken::RIGHT_SQUARE_BRACKET);
        iterator->next();
        return std::make_shared<Type::Array>(type);
    }

    auto typeId=expectIdentifier();
    auto typeName=std::make_shared<std::wstring>(typeId);    
    auto type=std::make_shared<Type>(typeName);
    auto lineNumber=iterator->lineNumber;
    typeChecker->subscribe(
        lineNumber,scope,type
    );
    iterator->next();
    return type;
}
