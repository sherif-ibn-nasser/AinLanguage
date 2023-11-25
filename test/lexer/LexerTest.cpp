#include "LexerLine.hpp"
#include "UnclosedCommentException.hpp"
#include <string>
#include <vector>
#include "LexerToken.hpp"
#include "SharedPtrTypes.hpp"
#include <iostream>
#include <memory>
#include <catch2/catch_all.hpp>
#include "Lexer.hpp"
#include "LinkedList.hpp"

class FakeAinFile: public IAinFile{
    public:
        
        FakeAinFile(std::vector<std::wstring> lines){
            this->path="FakeAinFile.ain";
            this->lines=lines;
        }

        void readAinFile() override{}

};

SCENARIO("Test Lexer","[LexerTest.cpp]"){

    /** Required when running all unit tests at same time*/
    LexerLine::openedDelimitedCommentsCount=0;
    
    WHEN("File has delimited comments"){
        THEN("Ignore them"){
            auto file=std::make_shared<FakeAinFile>(
                std::vector<std::wstring>{
                    L"/*line0/*",
                    L"/*line1*/*/",
                    L"  line2*/",
                    L"//line3 /*",
                    L"/*line4",
                    L"  line5*/id/*",
                    L"*/line6",
                }
            );
            Lexer lexer(file);
            auto tokens=lexer.getTokens();
            auto lines=file->getLines();
            std::vector<LexerToken> expectedTokens={
                LexerToken(LexerToken::COMMENT_TOKEN,lines[0]),
                LexerToken::EolToken(),
                LexerToken(LexerToken::COMMENT_TOKEN,lines[1]),
                LexerToken::EolToken(),
                LexerToken(LexerToken::COMMENT_TOKEN,lines[2]),
                LexerToken::EolToken(),
                LexerToken(LexerToken::COMMENT_TOKEN,lines[3]),
                LexerToken::EolToken(),
                LexerToken(LexerToken::COMMENT_TOKEN,lines[4]),
                LexerToken::EolToken(),
                LexerToken(LexerToken::COMMENT_TOKEN,L"  line5*/"),
                LexerToken::IdentifierToken(L"id"),
                LexerToken(LexerToken::COMMENT_TOKEN,L"/*"),
                LexerToken::EolToken(),
                LexerToken(LexerToken::COMMENT_TOKEN,L"*/"),
                LexerToken::IdentifierToken(L"line6"),
                LexerToken::EolToken(),
                LexerToken::EofToken()
            };

            REQUIRE(tokens->size==expectedTokens.size());
            tokens->forEachIndexed([=](SharedLexerToken token,int i){
                auto expectedToken=expectedTokens[i];
                REQUIRE(token->operator==(expectedToken));
            });
            REQUIRE(tokens->tail->val->getTokenType()==LexerToken::EOF_TOKEN);
        }
    }

    WHEN("A delimited comment is not closed"){
        THEN("Throw UnclosedCommentException"){
            auto file=std::make_shared<FakeAinFile>(
                std::vector<std::wstring>{
                    L"/*line0/*",
                    L"/*line1*/*/",
                }
            );
            auto matcher=Catch::Matchers::Predicate<UnclosedCommentException>(
                [=](UnclosedCommentException e){
                    return e.whatWstr().find(toWstring(file->getPath()))!=std::wstring::npos;
                }
            );
            REQUIRE_THROWS_MATCHES(
                Lexer(file),
                UnclosedCommentException,
                matcher
            );
        }
    }
}