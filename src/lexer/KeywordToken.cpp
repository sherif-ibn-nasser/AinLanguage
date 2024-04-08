#include <iostream>
#include "KeywordToken.hpp"

KeywordToken::KeywordToken(std::wstring val):
LexerToken(LexerToken::KEYWORD_TOKEN,val){}

bool KeywordToken::iskeyword(LexerToken t){
    KeywordToken keywords[]={
        MUT,LET,OPERATOR,FUN,RETURN,
        PACKAGE,IMPORT,NEW,THIS,CLASS,INTERFACE,OBJECT,
        DATA,ABSTRACT,OPEN,ENUM,ANNOTATION,
        PUBLIC,PRIVATE,PROTECTED,OVERRIDE,
        IF,/*ELSE_IF,*/ELSE,WHEN,
        FOR,DO,WHILE,BREAK,CONTINUE,
        THROW,TRY,CATCH,FINALLY,TRUE,FALSE
    };

    for(KeywordToken &k:keywords){
        if(t==k)
            return true;
    }
    return false;
}

bool KeywordToken::iskeyword(std::wstring val){
    return iskeyword(KeywordToken(val));
}

KeywordToken KeywordToken::LET=KeywordToken(L"احجز");
KeywordToken KeywordToken::MUT=KeywordToken(L"متغير");
KeywordToken KeywordToken::OPERATOR=KeywordToken(L"مؤثر");
KeywordToken KeywordToken::FUN=KeywordToken(L"دالة");
KeywordToken KeywordToken::RETURN=KeywordToken(L"أرجع");

KeywordToken KeywordToken::PACKAGE=KeywordToken(L"حزمة");
KeywordToken KeywordToken::IMPORT=KeywordToken(L"استيراد");
KeywordToken KeywordToken::NEW=KeywordToken(L"إنشاء");
KeywordToken KeywordToken::THIS=KeywordToken(L"هذا");
KeywordToken KeywordToken::CLASS=KeywordToken(L"تصنيف");
KeywordToken KeywordToken::INTERFACE=KeywordToken(L"وسيط");
KeywordToken KeywordToken::OBJECT=KeywordToken(L"كائن");

KeywordToken KeywordToken::DATA=KeywordToken(L"بيانات");
KeywordToken KeywordToken::ABSTRACT=KeywordToken(L"مجرد");
KeywordToken KeywordToken::OPEN=KeywordToken(L"مفتوح");
KeywordToken KeywordToken::ENUM=KeywordToken(L"سرد");
KeywordToken KeywordToken::ANNOTATION=KeywordToken(L"حاشية");

KeywordToken KeywordToken::PUBLIC=KeywordToken(L"تعميم");
KeywordToken KeywordToken::PRIVATE=KeywordToken(L"تخصيص");
KeywordToken KeywordToken::PROTECTED=KeywordToken(L"حماية");
KeywordToken KeywordToken::OVERRIDE=KeywordToken(L"هيمنة");

KeywordToken KeywordToken::IF=KeywordToken(L"لو");
//KeywordToken KeywordToken::ELSE_IF=KeywordToken(L"لكن لو");
KeywordToken KeywordToken::ELSE=KeywordToken(L"وإلا");
KeywordToken KeywordToken::WHEN=KeywordToken(L"عندما"); // as switch

KeywordToken KeywordToken::FOR=KeywordToken(L"لأجل");
KeywordToken KeywordToken::DO=KeywordToken(L"افعل");
KeywordToken KeywordToken::WHILE=KeywordToken(L"طالما");
KeywordToken KeywordToken::BREAK=KeywordToken(L"اقطع");
KeywordToken KeywordToken::CONTINUE=KeywordToken(L"أعد");

KeywordToken KeywordToken::THROW=KeywordToken(L"اعترض");
KeywordToken KeywordToken::TRY=KeywordToken(L"حاول");
KeywordToken KeywordToken::CATCH=KeywordToken(L"اقتنص");
KeywordToken KeywordToken::FINALLY=KeywordToken(L"بالنهاية");

// We may consider them as bool literal tokens, but we put them here instead
KeywordToken KeywordToken::TRUE=KeywordToken(L"صواب");
KeywordToken KeywordToken::FALSE=KeywordToken(L"خطأ");