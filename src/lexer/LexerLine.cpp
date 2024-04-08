#include<iostream>
#include<memory>
#include <string>
#include<vector>
#include<limits>
#include<algorithm>
#include"LinkedList.hpp"
#include"LexerLine.hpp"
#include"LiteralToken.hpp"
#include"NumberToken.hpp"
#include "SharedPtrTypes.hpp"
#include"SymbolToken.hpp"
#include"KeywordToken.hpp"
#include"wchar_t_helper.hpp"
#include"string_helper.hpp"
#include"MissingQuoteException.hpp"
#include"UnsupportedTokenException.hpp"
#include"IllegalUnderscoreException.hpp"
#include"OutOfRangeException.hpp"
#include"InvalidNumberSystemDigitException.hpp"
#include"InvalidIdentifierNameException.hpp"
#include"InvalidLengthCharacterLiteralException.hpp"
#include"InvalidEscapeSequenceException.hpp"
#include"InvalidUniversalCharacterCodeException.hpp"
#include"ContainsKufrOrUnsupportedCharacterException.hpp"

void LexerLine::checkIsKufrOrUnsupportedCharacter(const wchar_t &c){
    if(isKufrOrUnsupportedCharacter(c))
        throw ContainsKufrOrUnsupportedCharacterException(lineNumber,line);
}

int LexerLine::openedDelimitedCommentsCount=0;

LexerLine::LexerLine(std::wstring &line,int lineNumber){
    this->line=line;
    this->lineNumber=lineNumber;
    this->tokens=std::make_shared<LinkedList<SharedLexerToken>>();
}

bool LexerLine::isNotNullToken(SharedLexerToken token){
    if(token==nullptr)
        return false;
    tokens->insert(token);

    // move to the next token, start and end will be equal
    tokenStartIndex=++tokenEndIndex;
    return true;
}

std::wstring LexerLine::getCurrentTokenVal(){
    return line.substr(tokenStartIndex,tokenEndIndex-tokenStartIndex+1);
}

wchar_t LexerLine::charAt(int index){
    if(index<line.size())
        return line[index];
    
    return L'\0';
}

void LexerLine::tokenize(){

    while(openedDelimitedCommentsCount>0&&tokenStartIndex<line.size()){
        auto delimitedCommentToken=findDelimitedCommentToken();
        isNotNullToken(delimitedCommentToken);
    }

    while(tokenStartIndex<line.size()){
        
        auto stringOrCharToken=findStringOrCharToken();
        if(isNotNullToken(stringOrCharToken))
            continue;
        
        auto commentToken=findCommentToken();
        if(isNotNullToken(commentToken))
            continue;

        auto delimitedCommentToken=findDelimitedCommentToken();
        if(isNotNullToken(delimitedCommentToken))
            continue;
        
        auto symbolToken=findSymbolToken();
        if(isNotNullToken(symbolToken))
            continue;

        auto numberToken=findNumberToken();
        if(isNotNullToken(numberToken))
            continue;

        auto identifierOrKeywordToken=findIdentifierOrKeywordToken();
        if(isNotNullToken(identifierOrKeywordToken))
            continue;
        
        auto spaceToken=findSpaceToken();
        if(isNotNullToken(spaceToken))
            continue;
        
        // To skip spaces and tabs, move to the next token, start and end will be equal
        //tokenStartIndex=++tokenEndIndex;
    }

    auto eol=std::make_shared<LexerToken>(LexerToken::EOL_TOKEN,getCurrentTokenVal());
    tokens->insert(eol);
}

SharedLexerToken LexerLine::findStringOrCharToken(){
    
    auto quote=charAt(tokenStartIndex);
    auto isChar=quote==L'\'';
    auto isString=quote==L'\"';

    if(!isChar&&!isString)
        return nullptr;
    
    auto literalType=(isChar)?LiteralToken::CHAR:LiteralToken::STRING;
    
    // append every char in the line until finding another " or '
    std::wstring tokenVal=L"";
    tokenVal+=quote; // append first quote

    for(
        tokenEndIndex=tokenStartIndex+1 /*Start after first quote*/;
        tokenEndIndex<line.size();
        tokenEndIndex++
    ){
        auto currentChar=charAt(tokenEndIndex);

        /*If it's quote (" or ') return the token*/
        if(currentChar==quote){
            tokenVal+=quote;
            // the size should be 3 for the first, last quotes and the character between them
            if(isChar&&tokenVal.size()!=3)
                throw InvalidLengthCharacterLiteralException(lineNumber,getCurrentTokenVal());
            
            auto token=std::make_shared<LiteralToken>(literalType,tokenVal);
            return token;
        }

        /*Append if it is not a special character*/
        if(currentChar!=L'\\'){
            checkIsKufrOrUnsupportedCharacter(currentChar);
            tokenVal+=currentChar;
            continue;
        }

        tokenEndIndex++; // get next control char
        currentChar=charAt(tokenEndIndex);

        /*Unicode characters parsing*/
        if(currentChar==L'ي'){
            auto codePoint=line.substr(tokenEndIndex+1,4);
            try{
                auto c=getUnicodeCharacterFromCode(codePoint);
                checkIsKufrOrUnsupportedCharacter(c);
                tokenVal+=c;
                tokenEndIndex+=4; // skip the next 4 digits
                continue;
            }catch(std::invalid_argument e){
                throw InvalidUniversalCharacterCodeException(lineNumber,getCurrentTokenVal()+codePoint);
            }
        }

        /*Escape sequences*/
        try{
            auto es=getEscapeSequenceFromCharacter(currentChar);
            tokenVal+=es;
        }
        catch(std::invalid_argument e){
            throw InvalidEscapeSequenceException(lineNumber,getCurrentTokenVal());
        }

    }
    throw MissingQuoteException(lineNumber,getCurrentTokenVal());
}

SharedLexerToken LexerLine::findCommentToken(){

    auto commentIndex=line.find(L"//",tokenStartIndex);

    // may be there is a comment in the line but there're tokens before it, so return to tokenize them
    if(commentIndex!=tokenStartIndex)
        return nullptr;

    tokenEndIndex=line.size()-1;  // last character in comment
    
    return getCurrentTokenAsComment();
}

SharedLexerToken LexerLine::findDelimitedCommentToken(){


    if(openedDelimitedCommentsCount==0){

        auto openCommentIndex=line.find(L"/*",tokenStartIndex);

        // may be there is a comment in the line but there're tokens before it, so return to tokenize them
        if(openCommentIndex!=tokenStartIndex)
            return nullptr;

    }

    int i=tokenStartIndex;
    
    for(;i<line.size();i++){

        if(charAt(i)==L'/'&&charAt(i+1)==L'*'){
            i++;
            openedDelimitedCommentsCount++;
        }

        else if(charAt(i)==L'*'&&charAt(i+1)==L'/'){
            i++;
            openedDelimitedCommentsCount--;
        }

        if(openedDelimitedCommentsCount==0){
            i++;
            break;
        }
        
    }
    
    tokenEndIndex=i-1;

    return getCurrentTokenAsComment();

}


SharedLexerToken LexerLine::findSymbolToken(){
    
    /*
    find a multiple-symbol token (>=, <=, ==, !=, &&, ||),
    '::' static access operator ,'**' power operator, '**=' power assign operator,
    assignment operators and inc-dec operators
    */
    SymbolToken multipleSymbolTokens[]={
        SymbolToken::GREATER_EQUAL,
        SymbolToken::LESS_EQUAL,
        SymbolToken::EQUAL_EQUAL,
        SymbolToken::NOT_EQUAL,
        SymbolToken::LOGICAL_AND,
        SymbolToken::LOGICAL_OR,
        SymbolToken::PLUS_EQUAL,
        SymbolToken::MINUS_EQUAL,
        SymbolToken::STAR_EQUAL,
        SymbolToken::SLASH_EQUAL,
        SymbolToken::MODULO_EQUAL,
        SymbolToken::POWER_EQUAL,   // This should be before POWER to detect the '='
        SymbolToken::SHR_EQUAL,     // This should be before SHR   to detect the '='
        SymbolToken::SHL_EQUAL,     // This should be before SHL   to detect the '='
        SymbolToken::BIT_NOT_EQUAL,
        SymbolToken::BIT_AND_EQUAL,
        SymbolToken::XOR_EQUAL,
        SymbolToken::BIT_OR_EQUAL,
        SymbolToken::POWER,
        SymbolToken::DOUBLE_COLONS,
        SymbolToken::PLUS_PLUS,
        SymbolToken::MINUS_MINUS,
        SymbolToken::SHR,
        SymbolToken::SHL,
    };
    for(auto &s:multipleSymbolTokens){
        auto found=line.find(s.getVal(),tokenStartIndex);
        if(found!=tokenStartIndex)
            continue;
        tokenEndIndex+=s.getVal().size()-1; // skip remaining symbols
        return std::make_shared<SymbolToken>(s);
    }

    // find a single-symbol token
    auto c=charAt(tokenStartIndex);
    if(!isAinPunct(c)) // This excludes underscore
        return nullptr;
    
    auto token=std::make_shared<SymbolToken>(getCurrentTokenVal());
    return token;
}

SharedLexerToken LexerLine::findNumberToken(){

    if(!iswdigit(charAt(tokenStartIndex)))
        return nullptr;

    NUM_SYS numSys=NUM_SYS::DEC;
    auto numType=NumberToken::INT;

    /**
     * Skip after a digit array in a non-decimal number system.
     * If skipped, then change [numSys] and don't skip any more.
    */
    for(auto nS:{NUM_SYS::BIN, NUM_SYS::OCT, NUM_SYS::HEX}){
        skipAfterNonDecIntDigitArray(nS);
        if(tokenEndIndex!=tokenStartIndex){
            numSys=nS;
            break;
        }
    }

    /**
     * If numSys didn't change from previous loop,
     * then skip after a digit array in decimal and change the number type (int, double or float)
    */
    if(numSys==NUM_SYS::DEC)
        numType=skipAfterDecDigitArray();
    
    auto number=getCurrentTokenVal();

    // remove underscores
    removeUnderscores(&number);

    switch(numType){
        case NumberToken::DOUBLE:
            getDoubleNumberToken(&number);
            break;
        case NumberToken::FLOAT:
            getFloatNumberToken(&number);
            break;
        default: // INT
            getIntNumberToken(&number,&numType,numSys);
    }

    auto token=std::make_shared<NumberToken>(numType,number);
    return token;
}

void LexerLine::skipAfterNonDecIntDigitArray(NUM_SYS numSys){
    
    if(charAt(tokenStartIndex)!=L'0')
        return;
    
    auto numSysCharLine=charAt(tokenStartIndex+1);

    wchar_t numSysChar;
    switch(numSys)
    {
        case BIN:
            numSysChar=L'b';
            break;
        case OCT:
            numSysChar=L'o';
            break;
        case HEX:
            numSysChar=L'x';
            break;
        default:
            std::__throw_runtime_error("function `skipAfterNonDecNumSystemInt` is only for non-decimal numbers");
    }

    if(std::towlower(numSysCharLine)!=numSysChar)
        return;

    // to skip the zero and numSysChar
    tokenEndIndex++;
    skipAfterDigitArray(tokenEndIndex+1,numSys);

    auto stopChar=std::towlower(charAt(tokenEndIndex+1)); // where the skipping stopped

    // user types 0b12, ob1a, 0O128, ,0b12s, 0xfg etc.
    if (
        stopChar!=L'م'&&stopChar!=L'ص'
        &&
        (
            (iswxdigit(stopChar) && numSys!=NUM_SYS::HEX)
            ||
            isAinAlpha(stopChar)
        )
    )
        throw InvalidNumberSystemDigitException(lineNumber,getCurrentTokenVal()+(wchar_t)stopChar);
}

NumberToken::NUMBER_TYPE LexerLine::skipAfterDecDigitArray(){

    auto numType=NumberToken::INT;

    skipAfterDigitArray(tokenStartIndex);

    // number before dot may be treated as object, so check if after the dot is a digit to build the double
    if(charAt(tokenEndIndex+1)==L'.' && iswdigit(charAt(tokenEndIndex+2))){
        numType=NumberToken::DOUBLE;
        tokenEndIndex++; // add '.' symbol to the token
        skipAfterDigitArray(tokenEndIndex+1);
    }

    if(std::towlower(charAt(tokenEndIndex+1))==L'ق'){
        numType=NumberToken::DOUBLE; // may be doesn't have a dot, so reassign it to DOUBLE
        // add 'e' symbol to the token
        tokenEndIndex++;
        skipDigitsAfterExponent();
    }

    if(std::towlower(charAt(tokenEndIndex+1))==L'ع'){
        tokenEndIndex++; // add 'ع' symbol to the token
        if(charAt(tokenEndIndex+1)==L'4'){
            numType=NumberToken::FLOAT;
            tokenEndIndex++; // add '4' symbol to the token
        }
        else if(charAt(tokenEndIndex+1)==L'8'){
            tokenEndIndex++; // add '8' symbol to the token
        }
    }

    wchar_t stopChar=std::towlower(charAt(tokenEndIndex+1));

    if(!isAinAlpha(stopChar))
        return numType;

    /**
     * The number token has an alphabet (which isn't valid for decimal system numbers)
    */

   // numType is float or double
    if(numType!=NumberToken::INT)
        throw UnsupportedTokenException(lineNumber,getCurrentTokenVal()+stopChar); // append stopChar
    
    // numType is int
    // checking for stopChar after unsigned, byte and long types is in getIntNumberToken
    if(stopChar!=L'م'&&stopChar!=L'ص')
        throw InvalidIdentifierNameException(lineNumber,getCurrentTokenVal()+stopChar); // append stopChar
    
    // Nothing happened, numType will be unsigned byte, unsigned int, unsigned long, byte or long in getIntNumberToken
    return numType;
}

void LexerLine::skipDigitsAfterExponent(){
    if(charAt(tokenEndIndex+1)==L'-'||charAt(tokenEndIndex+1)==L'+') // sign of exponent
        tokenEndIndex++; // add the sign to token

    // when character after exponent is underscore
    auto nextChar=charAt(tokenEndIndex+1);
    if(nextChar==L'_')
        throw IllegalUnderscoreException(lineNumber,getCurrentTokenVal()+nextChar); // append nextChar

    // when character after exponent isn't a digit
    if(!iswdigit(nextChar))
        throw UnsupportedTokenException(lineNumber,getCurrentTokenVal()+nextChar); // append nextChar

    auto skipStart=tokenEndIndex+1;

    // skip after checking next char
    skipAfterDigitArray(skipStart);

    // if tokenEndIndex didn't change after skip, error will be thrown
    // this when number ends with exponent without numbers after 'e' symbol (e.g., 10e, 2e+, 3e-)
    if(tokenEndIndex==skipStart-1)
        throw UnsupportedTokenException(lineNumber,getCurrentTokenVal());
}

void LexerLine::skipAfterDigitArray(int startFrom,NUM_SYS numSys){
    auto isNumSysDigit=[&](wchar_t &&c){
        switch(numSys)
        {
            case BIN:
                return iswbdigit(c);
            case OCT:
                return iswodigit(c);
            case HEX:
                return (bool)std::iswxdigit(c);
            default:
                return (bool)std::iswdigit(c);
        }
    };

    // number may be 0b, 0o, 0x, so throw an error
    if(!isNumSysDigit(charAt(startFrom)))
        throw InvalidNumberSystemDigitException(lineNumber,getCurrentTokenVal()+charAt(startFrom)); // apend next char

    int i=startFrom+1;
    while(i<line.size() && (isNumSysDigit(charAt(i))||charAt(i)==L','))
        i++;

    tokenEndIndex=i-1;

    // must be no commas at the end of number
    if(charAt(tokenEndIndex)==L',')
        throw IllegalUnderscoreException(lineNumber,getCurrentTokenVal());

}

void LexerLine::getIntNumberToken(
    std::wstring* number,
    NumberToken::NUMBER_TYPE* numType,
    NUM_SYS numSys
){

    if(numSys!=DEC)
        *number=number->substr(2); // skip prefix to not struggle with stoull
    
    // TODO: May prevent leading zeros in integers
    unsigned long long num;
    try{
        num=std::stoull(*number,0,numSys);
    }catch(std::out_of_range e){
        throw OutOfRangeException(lineNumber,*number);
    }

    // May be int or long or unsigned of them

    /**
     * The unary minus from Parser may change the numType if the num reached the limits of int or long
    */

    if(charAt(tokenEndIndex+1)==L'م'){

        tokenEndIndex++; // append 'م' symbol to token
        
        if (charAt(tokenEndIndex+1)==L'8') {
            *numType=NumberToken::UNSIGNED_LONG;
            tokenEndIndex++; // append '8' symbol to token
        }
        else if (charAt(tokenEndIndex+1)==L'4') {
            if(num>std::numeric_limits<unsigned int>::max())
                throw OutOfRangeException(lineNumber,getCurrentTokenVal());
            *numType=NumberToken::UNSIGNED_INT;
            tokenEndIndex++; // append '4' symbol to token
        }
        else if (charAt(tokenEndIndex+1)==L'2') {
            if(num>std::numeric_limits<unsigned short>::max())
                throw OutOfRangeException(lineNumber,getCurrentTokenVal());
            *numType=NumberToken::UNSIGNED_SHORT;
            tokenEndIndex++; // append '2' symbol to token
        }
        else if (charAt(tokenEndIndex+1)==L'1') {
            if(num>std::numeric_limits<unsigned char>::max())
                throw OutOfRangeException(lineNumber,getCurrentTokenVal());
            *numType=NumberToken::UNSIGNED_BYTE;
            tokenEndIndex++; // append '1' symbol to token
        }
        else
            *numType=(num<=std::numeric_limits<unsigned int>::max())
            ?NumberToken::UNSIGNED_INT
            :NumberToken::UNSIGNED_LONG;
        
    }
    else if(charAt(tokenEndIndex+1)==L'ص'){
        tokenEndIndex++; // append 'ص' symbol to token
        
        if (charAt(tokenEndIndex+1)==L'8') {
            *numType=NumberToken::LONG;
            tokenEndIndex++; // append '8' symbol to token
        }
        else if (charAt(tokenEndIndex+1)==L'4') {
            if(num>std::numeric_limits<int>::max())
                throw OutOfRangeException(lineNumber,getCurrentTokenVal());
            *numType=NumberToken::INT;
            tokenEndIndex++; // append '4' symbol to token
        }
        else if (charAt(tokenEndIndex+1)==L'2') {
            if(num>std::numeric_limits<short>::max())
                throw OutOfRangeException(lineNumber,getCurrentTokenVal());
            *numType=NumberToken::SHORT;
            tokenEndIndex++; // append '4' symbol to token
        }
        else if (charAt(tokenEndIndex+1)==L'1') {
            if(num>std::numeric_limits<char>::max())
                throw OutOfRangeException(lineNumber,getCurrentTokenVal());
            *numType=NumberToken::BYTE;
            tokenEndIndex++; // append '1' symbol to token
        }
        else 
            *numType=(num<=std::numeric_limits<int>::max())
            ?NumberToken::INT
            :NumberToken::LONG;
    }
    else
        *numType=
            (num<=std::numeric_limits<int>::max())
            ?NumberToken::INT
            :(num<=std::numeric_limits<long long>::max())
            ?NumberToken::LONG
            :throw OutOfRangeException(lineNumber,getCurrentTokenVal());
    
    wchar_t stopChar=charAt(tokenEndIndex+1);
    
    if(isAinAlpha(stopChar))
        throw InvalidIdentifierNameException(lineNumber,getCurrentTokenVal()+stopChar); // append stopChar
    
    *number=std::to_wstring(num);
}

void LexerLine::getDoubleNumberToken(std::wstring* number){
    try{
        auto doubleNum=std::stod(*number);
    }
    catch(std::out_of_range e){
        throw OutOfRangeException(lineNumber,*number);
    }
}

void LexerLine::getFloatNumberToken(std::wstring* number){
    try{
        auto floatNum=std::stof(*number);
    }
    catch(std::out_of_range e){
        throw OutOfRangeException(lineNumber,*number);
    }
}

SharedLexerToken LexerLine::findIdentifierOrKeywordToken(){
    // Identifiers nad tokens don't start with digits, symbols or spaces
    if(!isAinAlpha(charAt(tokenStartIndex)))
        return nullptr;
    
    int i=tokenStartIndex;
    do i++;
    while(isAinAlpha(charAt(i))||iswdigit(charAt(i))); // if next char is alpha or digit add it

    tokenEndIndex=i-1;

    auto val=getCurrentTokenVal();

    auto tokenType=(KeywordToken::iskeyword(val))
    ?LexerToken::KEYWORD_TOKEN:LexerToken::IDENTIFIER_TOKEN;

    if(tokenType==LexerToken::IDENTIFIER_TOKEN){
        for(auto &c:val)
            checkIsKufrOrUnsupportedCharacter(c);
    }

    auto token=std::make_shared<LexerToken>(tokenType,val);
    return token;
}

SharedLexerToken LexerLine::findSpaceToken(){
    if(!iswempty(charAt(tokenStartIndex)))
        return nullptr;
    int i=tokenStartIndex;
    while(iswempty(charAt(i)))
        i++;
    
    tokenEndIndex=i-1;

    return std::make_shared<LexerToken>(LexerToken::SPACE_TOKEN,getCurrentTokenVal());
}

SharedLexerToken LexerLine::getCurrentTokenAsComment(){
    auto comment=getCurrentTokenVal();

    for(auto &c:comment)
        checkIsKufrOrUnsupportedCharacter(c);
    
    auto token=std::make_shared<LexerToken>(LexerToken::COMMENT_TOKEN,comment);

    return token;
}
