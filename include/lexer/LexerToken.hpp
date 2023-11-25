#pragma once
#include"SharedPtrTypes.hpp"

class LexerToken{
public:
    enum TOKEN_TYPE : int
    {
        BAD_TOKEN = -1,
        SYMBOL_TOKEN = 0,
        LITERAL_TOKEN = 1,
        COMMENT_TOKEN = 2,
        KEYWORD_TOKEN = 3,
        IDENTIFIER_TOKEN = 4,
        SPACE_TOKEN = 5,
        EOL_TOKEN = 6,
        EOF_TOKEN = 7,
    };
    LexerToken(TOKEN_TYPE tokenType, std::wstring val);
    TOKEN_TYPE getTokenType();
    std::wstring getVal();
    bool operator== (LexerToken &token) const;
    bool operator!= (LexerToken &token) const;
    static bool isNumberLiteral(SharedLexerToken token);
    static bool isStringLiteral(SharedLexerToken token);
    static std::wstring stringify(TOKEN_TYPE tokenType);
    static LexerToken IdentifierToken(std::wstring val=L"مُعرِّف");
    static LexerToken SpaceToken(std::wstring val=L"");
    static LexerToken EolToken();
    static LexerToken EofToken();
    virtual ~LexerToken();
private:
    TOKEN_TYPE tokenType;
    std::wstring val;
};
