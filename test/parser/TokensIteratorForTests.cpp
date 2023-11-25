#include "TokensIteratorForTests.hpp"
#include "SharedPtrTypes.hpp"
#include <memory>

SharedTokensIterator getTokensIterator(
    std::vector<LexerToken> tokens
){
    auto list=LinkedList<SharedLexerToken>();
    for(auto &token:tokens){
        list.insert(
            std::make_shared<LexerToken>(token)
        );
    }
    return std::make_shared<TokensIterator>(list);
}