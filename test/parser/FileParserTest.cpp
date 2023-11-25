#include "FileParser.hpp"
#include "BaseParser.hpp"
#include "LexerToken.hpp"
#include "PackageScope.hpp"
#include "FileScope.hpp"
#include "SharedPtrTypes.hpp"
#include "TokensIteratorForTests.hpp"
#include <catch2/catch_all.hpp>
#include <memory>

namespace{
    struct FakePackageParser:public BaseParser<SharedPackageScope>{
        FakePackageParser():BaseParser(nullptr,nullptr){}
        SharedPackageScope parse() override{
            return PackageScope::AIN_PACKAGE;
        }
    };

    struct FakeFunParser:public BaseParser<SharedFunScope>{
        FakeFunParser():BaseParser(nullptr,nullptr){}
        SharedFunScope parse() override{
            return nullptr;
        }
    };

    struct FakeClassParser:public BaseParser<SharedClassScope>{
        FakeClassParser():BaseParser(nullptr,nullptr){}
        SharedClassScope parse() override{
            return nullptr;
        }
    };

    struct FakeVarStatementParser:public BaseParser<SharedVarStm>{
        FakeVarStatementParser():BaseParser(nullptr,nullptr){}
        SharedVarStm parse() override{
            return nullptr;
        }
    };
}

TEST_CASE("Test file parser is adding file to parent package","[FileParserTest.cpp]"){
    auto filePath=L"somePath";
    auto fakePackageParser=std::make_shared<FakePackageParser>();
    auto fakeFunParser=std::make_shared<FakeFunParser>();
    auto fakeClassParser=std::make_shared<FakeClassParser>();
    auto fakeVarStmParser=std::make_shared<FakeVarStatementParser>();
    auto fakeFunParserProvider=[&](SharedTokensIterator,SharedBaseScope){
        return fakeFunParser;
    };
    auto fakeClassParserProvider=[&](SharedTokensIterator,SharedBaseScope){
        return fakeClassParser;
    };
    auto fakeVarStmParserProvider=[&](SharedTokensIterator,SharedBaseScope){
        return fakeVarStmParser;
    };
    auto parsedPackage=fakePackageParser->parse();
    FileParser fp(
        getTokensIterator({LexerToken::EofToken()}),
        filePath,
        fakePackageParser,
        fakeFunParserProvider,
        fakeClassParserProvider,
        fakeVarStmParserProvider
    );
    fp.parse();

    auto file=parsedPackage->findFileByPath(filePath);
    REQUIRE(file->getParentScope()==PackageScope::AIN_PACKAGE);
}