#include "LexerToken.hpp"
#include "SharedPtrTypes.hpp"
#include "FunParamParser.hpp"
#include "FunParam.hpp"
#include "SymbolToken.hpp"
#include "TokensIteratorForTests.hpp"
#include "PackageScope.hpp"
#include "Type.hpp"
#include "FileScope.hpp"
#include <catch2/catch_all.hpp>

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


TEST_CASE("Test function parameter parser","[FunParamParserTest.cpp]"){
    auto fakeTypeParser=std::make_shared<FakeTypeParser>();
    auto fakeTypeParserProvider=[&](SharedTokensIterator,SharedBaseScope){
        return fakeTypeParser;
    };
    fakeTypeParser->type=Type::INT;
    auto scope = std::make_shared<FileScope>(L"Test",PackageScope::AIN_PACKAGE);

    SECTION("parsing a function parameter"){
        auto iterator=getTokensIterator({
                LexerToken::IdentifierToken(L"param1"),
                SymbolToken::COLON,
                /**Fake parsed type*/
        });
        auto result=FunParamParser(
            iterator,
            scope,
            fakeTypeParserProvider
        ).parse();
        
        REQUIRE(fakeTypeParser->calledTimes==1);
        REQUIRE(*result->name==L"param1");
        REQUIRE(*result->type==*fakeTypeParser->type);
    }

    SECTION("throw UnexpectedTokenException when calling this parser's method and no param name"){
        auto iterator=getTokensIterator({
                SymbolToken::COLON,
                /**Fake parsed type*/
        });
        auto parser=FunParamParser(
            iterator,
            scope,
            fakeTypeParserProvider
        );
        
        REQUIRE_THROWS_AS(parser.parse(), UnexpectedTokenException);

    }

    SECTION("throw UnexpectedTokenException when calling this parser's method and no colon after param name"){
        auto iterator=getTokensIterator({
                LexerToken::IdentifierToken(L"param1"),
                /**Fake parsed type*/
        });
        auto parser=FunParamParser(
            iterator,
            scope,
            fakeTypeParserProvider
        );
        
        REQUIRE_THROWS_AS(parser.parse(), UnexpectedTokenException);

    }

}