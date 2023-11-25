#pragma once
#include "SharedPtrTypes.hpp"

class ILexerLine{
    protected:
        enum NUM_SYS{
            DEC=10,
            BIN=2,
            OCT=8,
            HEX=16,
        };
        SharedLinkedList<SharedLexerToken> tokens;
        std::wstring line;
        int lineNumber;

    public:
        virtual void tokenize()=0;
        SharedLinkedList<SharedLexerToken> getTokens(){return tokens;}
        int getLineNumber(){return lineNumber;}
};
