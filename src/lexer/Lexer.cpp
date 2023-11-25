#include <iostream>
#include <memory>
#include "Lexer.hpp"
#include "LexerToken.hpp"
#include "LinkedList.hpp"
#include "LexerLine.hpp"
#include "UnclosedCommentException.hpp"
#include "string_helper.hpp"
#include "wchar_t_helper.hpp"
#include "KeywordToken.hpp"
#include "SymbolToken.hpp"

Lexer::Lexer(SharedIAinFile ainFile):ainFile(ainFile){
    lexerLines=std::make_shared<std::vector<SharedILexerLine>>();

    auto lines=ainFile->getLines();

    int lineNumber=0;

    for(auto &line:lines){
        lineNumber++;

        auto nextLexerLine=std::make_shared<LexerLine>(line,lineNumber);
        nextLexerLine->tokenize();
        lexerLines->push_back(nextLexerLine);
    }

    if(LexerLine::openedDelimitedCommentsCount>0)
        throw UnclosedCommentException(toWstring(ainFile->getPath()));
    
}

SharedVector<SharedILexerLine> Lexer::getLexerLines(){
    return lexerLines;
}

SharedLinkedList<SharedLexerToken> Lexer::getTokens(){    
    auto tokens=std::make_shared<LinkedList<SharedLexerToken>>();
    for(auto &line:*lexerLines){
        auto lTokens=line->getTokens();
        tokens->insert(*lTokens);
    }
    tokens->insert(std::make_shared<LexerToken>(LexerToken::EOF_TOKEN,L""));
    return tokens;
}