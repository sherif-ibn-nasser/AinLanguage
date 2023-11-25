#include "PackageParser.hpp"
#include "PackageScope.hpp"
#include "BaseParser.hpp"
#include "KeywordToken.hpp"
#include "LexerToken.hpp"
#include "SymbolToken.hpp"
#include "UnexpectedTokenException.hpp"
#include "TokensIterator.hpp"
#include <memory>

SharedPackageScope PackageParser::parse(){
    auto currentPackage=std::dynamic_pointer_cast<PackageScope>(scope);
    
    if(!iterator->currentMatch(KeywordToken::PACKAGE))
        return currentPackage;
        
    do currentPackage=addPackageTo(currentPackage);
    while(iterator->currentMatch(SymbolToken::DOT));
    

    return currentPackage;
}

SharedPackageScope PackageParser::addPackageTo(SharedPackageScope parent){

    auto packageName=expectNextIdentifier();

    auto foundPackage=parent->findPackageByName(packageName);

    auto newPackage=(foundPackage)
    ?foundPackage
    :std::make_shared<PackageScope>(packageName,parent);

    if(!foundPackage)
        parent->addPackage(newPackage);
    
    iterator->next();

    return newPackage;
}