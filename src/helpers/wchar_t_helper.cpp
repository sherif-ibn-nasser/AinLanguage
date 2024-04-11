#include <iostream>
#include <algorithm>
#include "wchar_t_helper.hpp"
#include "LexerToken.hpp"

bool isAinPunct(const wchar_t &c)
{
    switch (c) {
        case L'؟':
        case L'؛':
        case L'،':
        case L'+':
        case L'-':
        case L'*':
        case L'/':
        case L'%':
        case L':':
        case L'|':
        case L'&':
        case L'^':
        case L'~':
        case L'=':
        case L'>':
        case L'<':
        case L'(':
        case L')':
        case L'[':
        case L']':
        case L'{':
        case L'}':
        case L'!':
        case L'.':
        case L'\"':
        case L'\'':
        case L'\\':
            return true;
    }
    return false;
}

bool isAinAlpha(const wchar_t &c)
{
    return !isAinPunct(c)&&!std::iswdigit(c)&&!iswempty(c)&&c!=L'\0';
}

bool iswempty(const wchar_t &c)
{
    return std::iswspace(c)||std::iswblank(c);
}

bool isExponentOperator(const wchar_t &c)
{
    return c==L'E'||c==L'e';
}

bool iswbdigit(const wchar_t &c){
    return c==L'0' || c==L'1';
}

bool iswodigit(const wchar_t &c){
    return c>=L'0' && c<=L'7';
}

bool isNumSystemChar(const wchar_t &c){
    return c==L'x'||c==L'X'||c==L'b'||c==L'B'||c==L'o'||c==L'O';
}

bool isKufrOrUnsupportedCharacter(const wchar_t &c){
    

    auto isInChars=std::find(kufrAndInvalidChars.begin(),kufrAndInvalidChars.end(),c);

    if(isInChars!=kufrAndInvalidChars.end()) // found
        return true;

    for(auto &range:kufrAndInvalidCharsRanges){
        if(c>=range.first&&c<=range.second)
            return true; // found
    }

    return false;
}