#include "KeywordToken.hpp"
#include "LexerToken.hpp"
#include "PackageScope.hpp"
#include "FileScope.hpp"
#include "SharedPtrTypes.hpp"
#include "SymbolToken.hpp"
#include "TokensIteratorForTests.hpp"
#include "VarDeclParser.hpp"
#include "VarDecl.hpp"
#include "Type.hpp"
#include <catch2/catch_all.hpp>
#include <vector>

namespace {
    struct FakeTypeParser:public BaseParser<SharedType>{
        int calledTimes=0;
        SharedType type;
        FakeTypeParser():BaseParser(nullptr,nullptr){}
        SharedType parse()override{
            calledTimes++;
            return type;
        }
    };
}

TEST_CASE("VarDeclParser tests","[VarDeclParserTest.cpp]"){
    auto fakeTypeParser=std::make_shared<FakeTypeParser>();
    fakeTypeParser->type=Type::INT;
    auto fakeTypeParserProvider=[&](SharedTokensIterator,SharedBaseScope){
        return fakeTypeParser;
    };
    
    auto scope = std::make_shared<FileScope>(L"Test",PackageScope::AIN_PACKAGE);

    auto NAME=LexerToken::IdentifierToken(L"اسم_المتغير");

    SECTION("parse var keyword"){
        std::vector<LexerToken> tokens={
            KeywordToken::MUT,
            NAME
        };
        SharedVarDecl result;

        SECTION("With implicit type"){
            auto iterator=getTokensIterator(tokens);
            VarDeclParser parser(
                iterator,scope,fakeTypeParserProvider
            );
            auto result=parser.parse();

            REQUIRE(fakeTypeParser->calledTimes==0);
            REQUIRE(result->hasImplicitType());
            REQUIRE(*result->name==NAME.getVal());
            REQUIRE_FALSE(*result->isVal);
        }

        SECTION("With explicit type"){
            tokens.push_back(SymbolToken::COLON);

            auto iterator=getTokensIterator(tokens);
            VarDeclParser parser(
                iterator,scope,fakeTypeParserProvider
            );
            auto result=parser.parse();

            REQUIRE(fakeTypeParser->calledTimes==1);
            REQUIRE_FALSE(result->hasImplicitType());
            REQUIRE(result->type==fakeTypeParser->type);
            REQUIRE(*result->name==NAME.getVal());
            REQUIRE_FALSE(*result->isVal);
        }

    }

    SECTION("parse val keyword"){
        std::vector<LexerToken> tokens={
            KeywordToken::LET,
            NAME
        };
        SharedVarDecl result;

        SECTION("With implicit type"){
            auto iterator=getTokensIterator(tokens);
            VarDeclParser parser(
                iterator,scope,fakeTypeParserProvider
            );
            result=parser.parse();

            REQUIRE(fakeTypeParser->calledTimes==0);
            REQUIRE(result->hasImplicitType());
            REQUIRE(*result->name==NAME.getVal());
            REQUIRE(*result->isVal);
        }

        SECTION("With explicit type"){
            tokens.push_back(SymbolToken::COLON);

            auto iterator=getTokensIterator(tokens);
            VarDeclParser parser(
                iterator,scope,fakeTypeParserProvider
            );
            result=parser.parse();

            REQUIRE(fakeTypeParser->calledTimes==1);
            REQUIRE_FALSE(result->hasImplicitType());
            REQUIRE(result->type==fakeTypeParser->type);
            REQUIRE(*result->name==NAME.getVal());
            REQUIRE(*result->isVal);
        }

    }
}