#pragma once
#include "LinkedList.hpp"
#include "LexerToken.hpp"
#include "SharedPtrTypes.hpp"
struct TokensIterator{
    int lineNumber;
    SharedLinkedListNode<SharedLexerToken> current;
    
    TokensIterator(LinkedList<SharedLexerToken> tokensList);

    SharedLexerToken next();
    bool currentMatch(LexerToken expected);
    bool nextMatch(LexerToken expected);
    SharedLexerToken currentToken();
    std::wstring currentVal();
    LexerToken::TOKEN_TYPE currentTokenType();
};