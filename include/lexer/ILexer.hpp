#pragma once
#include "SharedPtrTypes.hpp"

class ILexer{
        
    public:
        virtual SharedVector<SharedILexerLine> getLexerLines()=0;
        virtual SharedLinkedList<SharedLexerToken> getTokens()=0;
};