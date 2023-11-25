#pragma once
#include <iostream>
#include <memory>
#include "IAinFile.hpp"
#include "ILexer.hpp"
#include "LexerLine.hpp"

class Lexer:public ILexer{
    private:
        SharedIAinFile ainFile;
        SharedVector<SharedILexerLine> lexerLines;
        
    public:
        Lexer(SharedIAinFile ainFile);
        SharedVector<SharedILexerLine> getLexerLines() override;
        SharedLinkedList<SharedLexerToken> getTokens() override;
};
