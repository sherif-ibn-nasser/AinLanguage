#include "LexerToken.hpp"
#include "SharedPtrTypes.hpp"
#include "SymbolToken.hpp"
#include "TokensIteratorForTests.hpp"
#include "TypeChecker.hpp"
#include "PackageScope.hpp"
#include "FileScope.hpp"
#include "Type.hpp"
#include "TypeParser.hpp"
#include "UnexpectedTokenException.hpp"
#include <catch2/catch_all.hpp>
#include <memory>
#include <vector>

namespace{
    struct FakeTypeChecker:public Semantics::ITypeChecker{
        int calledTimes=0;
        
        void subscribe(int lineNumber,SharedBaseScope searchScope,SharedType type)override{
            calledTimes++;
            type->setClassScope(Type::INT->getClassScope());
        }

        void check()override{};
    };

}

TEST_CASE("TypeParser tests","[TypeParserTest.cpp]"){

    auto fakeTypeChecker=std::make_shared<FakeTypeChecker>();

    auto scope = std::make_shared<FileScope>(L"Test",PackageScope::AIN_PACKAGE);

    SECTION("parsing a type"){
        TypeParser parser(
            getTokensIterator({
                LexerToken::IdentifierToken(*Type::INT->getName())
            }),
            scope,
            fakeTypeChecker
        );

        auto type=parser.parse();

        REQUIRE(*type==*Type::INT);
        REQUIRE(fakeTypeChecker->calledTimes==1);
        /*TODO test with generic and nullable types*/
    }
    
    SECTION("Throw UnexpectedTokenException when calling parse without providing an identifier for the type"){
        TypeParser parser(getTokensIterator({}),scope,fakeTypeChecker);

        REQUIRE_THROWS_AS(parser.parse(), UnexpectedTokenException);
        REQUIRE(fakeTypeChecker->calledTimes==0);
    }


    SECTION("parsing an array"){
        TypeParser parser(
            getTokensIterator({
                SymbolToken::LEFT_SQUARE_BRACKET,
                LexerToken::IdentifierToken(*Type::INT->getName()),
                SymbolToken::RIGHT_SQUARE_BRACKET
            }),
            scope,
            fakeTypeChecker
        );

        auto type=parser.parse();

        REQUIRE(*type==Type::Array(Type::INT));
        REQUIRE(fakeTypeChecker->calledTimes==1);
        
    }
    
    SECTION("parsing a 2d array"){
        TypeParser parser(
            getTokensIterator({
                SymbolToken::LEFT_SQUARE_BRACKET,
                SymbolToken::LEFT_SQUARE_BRACKET,
                LexerToken::IdentifierToken(*Type::INT->getName()),
                SymbolToken::RIGHT_SQUARE_BRACKET,
                SymbolToken::RIGHT_SQUARE_BRACKET,
            }),
            scope,
            fakeTypeChecker
        );

        auto type=parser.parse();

        REQUIRE(*type==Type::Array(std::make_shared<Type::Array>(Type::INT)));
        REQUIRE(fakeTypeChecker->calledTimes==1);

    }

    SECTION("Throw UnexpectedTokenException when no type for array"){
        TypeParser parser(
            getTokensIterator({
                SymbolToken::LEFT_SQUARE_BRACKET,
                SymbolToken::RIGHT_SQUARE_BRACKET
            }),
            scope,
            fakeTypeChecker
        );

        REQUIRE_THROWS_AS(parser.parse(), UnexpectedTokenException);
        REQUIRE(fakeTypeChecker->calledTimes==0);

    }
}