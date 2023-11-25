#include "BaseParser.hpp"
#include "OperatorFunctions.hpp"
#include "ParserProvidersAliases.hpp"
#include "StringMaker.hpp"
#include <catch2/catch_all.hpp>
#include <memory>
#include <string>
#include <vector>
#include "ConflictingDeclarationException.hpp"
#include "FileScope.hpp"
#include "FunDeclParser.hpp"
#include "FunParam.hpp"
#include "FunDecl.hpp"
#include "KeywordToken.hpp"
#include "LexerToken.hpp"
#include "InvalidOperatorFunDeclarationException.hpp"
#include "PackageScope.hpp"
#include "SharedPtrTypes.hpp"
#include "SymbolToken.hpp"
#include "Type.hpp"
#include "TypeParser.hpp"
#include "TokensIteratorForTests.hpp"
#include "UnexpectedTokenException.hpp"

namespace{
    struct FakeTypeParser:public BaseParser<SharedType>{
        int calledTimes=0;
        SharedType type;
        FakeTypeParser():BaseParser(nullptr,nullptr){}
        SharedType parse()override{
            calledTimes++;
            return type;
        }
    };

    struct FakeFunParamParser:public BaseParser<SharedFunParam>{
        int calledTimes=0;
        std::vector<FunParam> params;
        FakeFunParamParser():BaseParser(nullptr,nullptr){}
        SharedFunParam parse()override{
            auto param=std::make_shared<FunParam>(params[calledTimes]);
            calledTimes++;
            return param;
        }

        void addParam(std::wstring name,SharedType type){
            params.push_back(
                FunParam(
                    std::make_shared<std::wstring>(name),
                    type
                )
            );
        }
    };
}

TEST_CASE("FunDeclParser tests", "[FunDeclParserTest.cpp]"){
    
    auto fakeTypeParser=std::make_shared<FakeTypeParser>();
    auto fakeTypeParserProvider=[&](SharedTokensIterator,SharedBaseScope){
        return fakeTypeParser;
    };

    auto fakeFunParamParser=std::make_shared<FakeFunParamParser>();
    auto fakeFunParamParserProvider=[&](SharedTokensIterator,SharedBaseScope,TypeParserProvider){
        return fakeFunParamParser;
    };

    auto FUN_NAME=LexerToken::IdentifierToken(OperatorFunctions::PLUS_NAME);
    auto PARAM1_NAME=L"رقم";
    auto PARAM2_NAME=L"2رقم";

    auto scope = std::make_shared<FileScope>(L"Test",PackageScope::AIN_PACKAGE);

    SECTION("parse function declaration"){
        std::vector<LexerToken> tokens={
            KeywordToken::FUN,
            FUN_NAME,
            SymbolToken::LEFT_PARENTHESIS,
            /**Fake parsed param*/
            SymbolToken::COMMA,
            /**Fake parsed param*/
            SymbolToken::COMMA,
            SymbolToken::RIGHT_PARENTHESIS
        };
        // from above
        fakeFunParamParser->addParam(PARAM1_NAME, Type::INT);
        fakeFunParamParser->addParam(PARAM2_NAME, Type::DOUBLE);

        auto parse=[&](){
            auto iterator=getTokensIterator(tokens);

            FunDeclParser parser(
                iterator,
                scope,
                fakeTypeParserProvider,
                fakeFunParamParserProvider
            );

            auto result=parser.parse();

            REQUIRE(result != nullptr);

            REQUIRE(*result->name == FUN_NAME.getVal());
            REQUIRE(*result->isOperator == false);
            REQUIRE(fakeFunParamParser->calledTimes==2);
            REQUIRE(result->params->size() == 2);
            REQUIRE(*result->params->at(0)==fakeFunParamParser->params[0]);
            REQUIRE(*result->params->at(1)==fakeFunParamParser->params[1]);
            return result;
        };

        SECTION("with implicit return type"){
            auto result=parse();
            REQUIRE(result->hasImplicitReturnType());
            REQUIRE(fakeTypeParser->calledTimes==0);
        }

        SECTION("with explicit return type") {
            tokens.push_back(SymbolToken::COLON);
            fakeTypeParser->type=Type::DOUBLE;
            auto result=parse();
            REQUIRE_FALSE(result->hasImplicitReturnType());
            REQUIRE(fakeTypeParser->calledTimes==1);
            REQUIRE(*result->returnType == *fakeTypeParser->type);
        }
    }

    SECTION("parse operator function declaration") {
        auto iterator=getTokensIterator({
            KeywordToken::FUN,
            KeywordToken::OPERATOR,
            FUN_NAME,
            SymbolToken::LEFT_PARENTHESIS,
            /**Fake parsed param*/
            SymbolToken::COMMA, // add this comma as the actual code checks for right parenthesis before params
            SymbolToken::RIGHT_PARENTHESIS,
            SymbolToken::COLON,
            /**Fake parsed return type*/
        });
        fakeFunParamParser->addParam(PARAM1_NAME, Type::INT);
        fakeTypeParser->type=Type::DOUBLE;

        FunDeclParser parser(
            iterator,
            scope,
            fakeTypeParserProvider,
            fakeFunParamParserProvider
        );

        auto result = parser.parse();

        REQUIRE(result != nullptr);
        REQUIRE(*result->name == FUN_NAME.getVal());
        REQUIRE(*result->returnType == *fakeTypeParser->type);
        REQUIRE(*result->isOperator == true);
        REQUIRE(result->params->size() == fakeFunParamParser->params.size());
        REQUIRE(*result->params->at(0) == fakeFunParamParser->params[0]);
    }

    SECTION("throw ConflictingDeclarationException if function parameters have the same name") {
        auto iterator=getTokensIterator({
            KeywordToken::FUN,
            FUN_NAME,
            SymbolToken::LEFT_PARENTHESIS,
            /**Fake parsed param*/
            SymbolToken::COMMA,
            /**Fake parsed param*/
            SymbolToken::COMMA, // add this comma as the actual code checks for right parenthesis before params
            SymbolToken::RIGHT_PARENTHESIS,
            SymbolToken::COLON,
            /**Fake parsed return type*/
        });

        fakeFunParamParser->addParam(PARAM1_NAME, Type::INT);
        fakeFunParamParser->addParam(PARAM1_NAME, Type::DOUBLE);

        FunDeclParser parser(
            iterator,
            scope,
            fakeTypeParserProvider,
            fakeFunParamParserProvider
        );

        REQUIRE_THROWS_AS(parser.parse(), ConflictingDeclarationException);
    }

    SECTION("throw UnexpectedTokenException if one of tokens is missing"){
        std::vector<LexerToken> trueDecl={
            KeywordToken::FUN,
            FUN_NAME,
            SymbolToken::LEFT_PARENTHESIS,
            SymbolToken::RIGHT_PARENTHESIS,
            LexerToken::EolToken()
        };

        std::vector<int> missingTokensIndicies={
            1,  // FUN_NAME
            2,  // SymbolToken::LEFT_PARENTHESIS,
            3,  // SymbolToken::RIGHT_PARENTHESIS
        };

        auto removeAndAssert=[&](int missingTokenIndex){

            auto invalidDecl=trueDecl;

            invalidDecl.erase(invalidDecl.begin() + missingTokenIndex);

            auto iterator=getTokensIterator(invalidDecl);

            FunDeclParser parser(
                iterator,
                scope,
                fakeTypeParserProvider,
                fakeFunParamParserProvider
            );

            REQUIRE_THROWS_AS(parser.parse(), UnexpectedTokenException);

        };

        // add this param, as when RIGHT_PARENTHESIS is missing, it will try to parse a param,
        // so avoid nullptr exception with the fake param parser
        fakeFunParamParser->addParam(PARAM1_NAME, Type::INT);

        
        for(auto missingTokenIndex:missingTokensIndicies){
            removeAndAssert(missingTokenIndex);
        }
       
    }

}