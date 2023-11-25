#include "PackageParser.hpp"
#include "SharedPtrTypes.hpp"
#include "LexerToken.hpp"
#include "LinkedList.hpp"
#include "SymbolToken.hpp"
#include "KeywordToken.hpp"
#include "TokensIterator.hpp"
#include "PackageScope.hpp"
#include "TokensIteratorForTests.hpp"
#include "UnexpectedTokenException.hpp"
#include <catch2/catch_all.hpp>
#include <initializer_list>
#include <memory>
#include <string>

TEST_CASE("Test package parser","[PackageParserTest.cpp]"){
    auto eol=LexerToken::EolToken();
    auto eof=LexerToken::EofToken();
    auto packageKeyword=KeywordToken::PACKAGE;
    auto dot=SymbolToken::DOT;
    auto comma=SymbolToken::COMMA;
    auto package1=LexerToken::IdentifierToken(L"حزمة1");
    auto package2=LexerToken::IdentifierToken(L"حزمة2");
    auto package3=LexerToken::IdentifierToken(L"حزمة3");
    auto importKeyword=KeywordToken::IMPORT;
    

    SECTION("no packages keyword in iterator, will return Ain package"){

        auto iterator=getTokensIterator({importKeyword});

        auto parser=std::make_shared<PackageParser>(iterator,PackageScope::AIN_PACKAGE);

        auto returnedPackage=parser->parse();

        REQUIRE(returnedPackage==PackageScope::AIN_PACKAGE);

        SECTION("the currentNode of iterator should be next token to parse"){
            REQUIRE(iterator->currentToken()->operator==(importKeyword));
        }
    };

    SECTION("when there is package keyword"){
        auto iterator=getTokensIterator(
            {
                packageKeyword,
                package1,
                dot,
                package2,
                eol,
                importKeyword
            }
        );

        auto parser=std::make_shared<PackageParser>(iterator,PackageScope::AIN_PACKAGE);

        auto returnedPackage=parser->parse();
        auto returnedPackageParent=std::dynamic_pointer_cast<PackageScope>(
            returnedPackage->getParentScope()
        );
        auto returnedPackageParentParent=std::dynamic_pointer_cast<PackageScope>(
            returnedPackageParent->getParentScope()
        );
        REQUIRE(
            returnedPackageParentParent
            ==
            PackageScope::AIN_PACKAGE
        );
        REQUIRE(
            returnedPackageParent
            ==
            PackageScope::AIN_PACKAGE
            ->findPackageByName(package1.getVal())
        );
        REQUIRE(
            returnedPackage
            ==
            PackageScope::AIN_PACKAGE
            ->findPackageByName(package1.getVal())
            ->findPackageByName(package2.getVal())
        );

        SECTION("the currentNode of iterator should be next token to parse"){
            REQUIRE(iterator->currentToken()->operator==(importKeyword));
        }

        SECTION("when re-parse package (in different file), should return exist packages"){

            auto iterator2=getTokensIterator(
                {
                    packageKeyword,
                    package1,
                    dot,
                    package2,
                    eol,
                    dot,
                    package3,
                    eol
                }
            );

            auto parser2=std::make_shared<PackageParser>(iterator2,PackageScope::AIN_PACKAGE);

            auto returnedPackage2=parser2->parse();
            auto returnedPackage2Parent=std::dynamic_pointer_cast<PackageScope>(
                returnedPackage2->getParentScope()
            );

            REQUIRE(
                returnedPackage2
                ==
                PackageScope::AIN_PACKAGE
                ->findPackageByName(package1.getVal())
                ->findPackageByName(package2.getVal())
                ->findPackageByName(package3.getVal())
            );
            REQUIRE(
                returnedPackage2Parent
                ==
                PackageScope::AIN_PACKAGE
                ->findPackageByName(package1.getVal())
                ->findPackageByName(package2.getVal())
            );
        }
    };

    SECTION("Testing when unexpected token is after dot after package name"){
        
        auto iterator=getTokensIterator(
            {
                eol,
                packageKeyword,
                package1,
                dot,
                packageKeyword,
                eol
            }
        );

        auto parser=std::make_shared<PackageParser>(iterator,PackageScope::AIN_PACKAGE);

        auto matcher=Catch::Matchers::Predicate<UnexpectedTokenException>(
            [&](UnexpectedTokenException e){
                auto npos=std::wstring::npos;
                auto hasIdentifier=e.whatWstr().find(
                    LexerToken::stringify(LexerToken::IDENTIFIER_TOKEN)
                );
                auto hasKeyword=e.whatWstr().find(
                    LexerToken::stringify(LexerToken::KEYWORD_TOKEN)
                );
                auto lineNumberMatches=e.lineNumber==2;
                return lineNumberMatches&&hasIdentifier!=npos&&hasKeyword!=npos;
            }
        );
        REQUIRE_THROWS_MATCHES(
            parser->parse(),
            UnexpectedTokenException,
            matcher
        );

    };

}