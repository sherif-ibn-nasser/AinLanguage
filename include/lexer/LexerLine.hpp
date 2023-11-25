#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include "LexerToken.hpp"
#include "NumberToken.hpp"
#include "ILexerLine.hpp"

class LexerLine:public ILexerLine{
    private:

        /**
         * @brief The start index of the current token in the line (first char in the token)
        */
        int tokenStartIndex=0;

        /**
         * @brief The end index of the current token in the line (last char in the token)
        */
        int tokenEndIndex=0;

        /**
         * @brief Get the token value from [tokenStartIndex] to [tokenEndIndex]
        */
        std::wstring getCurrentTokenVal();

        /**
         * @brief Get the character in line of at [index]
        */
        wchar_t charAt(int index);

        bool isNotNullToken(SharedLexerToken token);
        SharedLexerToken findStringOrCharToken();
        SharedLexerToken findCommentToken();
        SharedLexerToken findDelimitedCommentToken();
        SharedLexerToken findSymbolToken();
        SharedLexerToken findNumberToken();
        SharedLexerToken findIdentifierOrKeywordToken();
        SharedLexerToken findSpaceToken();
        SharedLexerToken getCurrentTokenAsComment();
        void skipAfterNonDecIntDigitArray(NUM_SYS numSys);
        NumberToken::NUMBER_TYPE skipAfterDecDigitArray();
        // Skip digits after finding an 'e' in a double number token starting from [tokenEndIndex]
        void skipDigitsAfterExponent();
        void skipAfterDigitArray(int startFrom,NUM_SYS numSys=NUM_SYS::DEC); // Default is decimal
        void getIntNumberToken(std::wstring* number,NumberToken::NUMBER_TYPE* numType,NUM_SYS numSys);
        void getDoubleNumberToken(std::wstring* number);
        void getFloatNumberToken(std::wstring* number);
        void checkIsKufrOrUnsupportedCharacter(const wchar_t &c);
    public:
        static int openedDelimitedCommentsCount;
        LexerLine(std::wstring &line,int lineNumber);
        void tokenize()override;
};
