#pragma once
#include "SharedPtrTypes.hpp"
#include "SymbolToken.hpp"
#include "TokensIterator.hpp"
#include "UnexpectedTokenException.hpp"
#include <string>

template<typename T>
class BaseParser{
    protected:
        SharedTokensIterator iterator;
        SharedBaseScope scope;
        std::wstring expectIdentifier();
        std::wstring expectNextIdentifier();
        void expectSymbol(SymbolToken symbol);
        void expectNextSymbol(SymbolToken symbol);
    
    public:
        BaseParser<T>(
            SharedTokensIterator iterator,
            SharedBaseScope scope
        );
        virtual T parse()=0;
};

template<typename T>
BaseParser<T>::BaseParser(
    SharedTokensIterator iterator,
    SharedBaseScope scope
):iterator(iterator),scope(scope){}


template<typename T>
std::wstring BaseParser<T>::expectIdentifier(){
    if(iterator->currentTokenType()!=LexerToken::IDENTIFIER_TOKEN)
        throw UnexpectedTokenException(
            iterator->lineNumber,
            LexerToken::stringify(LexerToken::IDENTIFIER_TOKEN),
            LexerToken::stringify(iterator->currentTokenType())+L" "+iterator->currentVal()
        );
    
    return iterator->currentVal();
}


template<typename T>
std::wstring BaseParser<T>::expectNextIdentifier(){
    iterator->next();
    return expectIdentifier();
}

template<typename T>
void BaseParser<T>::expectSymbol(SymbolToken symbol){
    if(!iterator->currentMatch(symbol))
        throw UnexpectedTokenException(
            iterator->lineNumber,
            LexerToken::stringify(LexerToken::SYMBOL_TOKEN)+L" "+symbol.getVal(),
            LexerToken::stringify(iterator->currentTokenType())+L" "+iterator->currentVal()
        );
}

template<typename T>
void BaseParser<T>::expectNextSymbol(SymbolToken symbol){
    iterator->next();
    expectSymbol(symbol);
}