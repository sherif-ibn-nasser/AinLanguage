#include "TokensIterator.hpp"

TokensIterator::TokensIterator(LinkedList<SharedLexerToken> tokensList)
:current(tokensList.head),lineNumber(1){
    auto tokenType=currentTokenType();
    
    // skip those
    if(
        tokenType!=LexerToken::COMMENT_TOKEN
        &&
        tokenType!=LexerToken::SPACE_TOKEN
        &&
        tokenType!=LexerToken::EOL_TOKEN
    )
        return;
    
    if(tokenType==LexerToken::EOL_TOKEN)
        lineNumber++;
    
    next();
}

SharedLexerToken TokensIterator::next(){
    current=current->next;
    auto tokenType=currentTokenType();

    // skip those
    if(
        tokenType!=LexerToken::COMMENT_TOKEN
        &&
        tokenType!=LexerToken::SPACE_TOKEN
        &&
        tokenType!=LexerToken::EOL_TOKEN
    )
        return currentToken();
    

    if(tokenType==LexerToken::EOL_TOKEN)
        lineNumber++;
    
    return next();
}

bool TokensIterator::currentMatch(LexerToken expected){
    return *currentToken()==expected;
}

bool TokensIterator::nextMatch(LexerToken expected){
    next();
    return currentMatch(expected);
}

SharedLexerToken TokensIterator::currentToken(){
    return (current)
    ?current->val
    :std::make_shared<LexerToken>(LexerToken(LexerToken::EOF_TOKEN,L""));
}

std::wstring TokensIterator::currentVal(){
    return currentToken()->getVal();
}

LexerToken::TOKEN_TYPE TokensIterator::currentTokenType(){
    return currentToken()->getTokenType();
}
