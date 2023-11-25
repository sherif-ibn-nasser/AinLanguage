#include "TokensIterator.hpp"
#include "KeywordToken.hpp"
#include "LinkedList.hpp"
#include "SharedPtrTypes.hpp"
#include "SymbolToken.hpp"
#include <catch2/catch_all.hpp>
#include <memory>

TEST_CASE("Test tokens iterator","[TokensIteratorTest.cpp]"){
    LinkedList<SharedLexerToken> list;
    auto space=std::make_shared<LexerToken>(LexerToken(LexerToken::SPACE_TOKEN,L" "));
    auto comment=std::make_shared<LexerToken>(LexerToken(LexerToken::COMMENT_TOKEN,L"// هذا تعليق"));
    auto eol=std::make_shared<LexerToken>(LexerToken(LexerToken::EOL_TOKEN,L""));
    auto eof=std::make_shared<LexerToken>(LexerToken(LexerToken::EOF_TOKEN,L""));
    auto packageKeyword=std::make_shared<LexerToken>(KeywordToken::PACKAGE);
    auto dot=std::make_shared<LexerToken>(SymbolToken::DOT);
    auto identifier=std::make_shared<LexerToken>(LexerToken(LexerToken::IDENTIFIER_TOKEN,L"مُعرِّف"));
    auto identifier2=std::make_shared<LexerToken>(LexerToken(LexerToken::IDENTIFIER_TOKEN,L"2مُعرِّف"));
    list.insert(space);
    list.insert(space);
    list.insert(eol);
    list.insert(comment);
    list.insert(eol);
    list.insert(packageKeyword);
    list.insert(space);
    list.insert(comment);
    list.insert(eol);
    list.insert(identifier);
    list.insert(eol);
    list.insert(dot);
    list.insert(identifier2);
    list.insert(eof);
    TokensIterator iterator(list);

    SECTION("first token shouldn't be space or eol or a comment"){
        REQUIRE(iterator.currentMatch(*packageKeyword));
        SECTION("calling next will skip spaces, eols and comments and not eof"){
            iterator.next();
            REQUIRE(iterator.currentMatch(*identifier));
            iterator.next();
            REQUIRE(iterator.currentMatch(*dot));
            iterator.next();
            REQUIRE(iterator.currentMatch(*identifier2));
            SECTION("returns eof at the end"){
                iterator.next();
                REQUIRE(iterator.currentMatch(*eof));
                iterator.next();
                REQUIRE(iterator.currentMatch(*eof));
            };
        };
    };
}