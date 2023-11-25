#include "FunParam.hpp"
#include "FunParser.hpp"
#include "SharedPtrTypes.hpp"
#include "BaseParser.hpp"
#include "TokensIteratorForTests.hpp"
#include "Variable.hpp"
#include "FunParam.hpp"
#include "FunDecl.hpp"
#include "FunScope.hpp"
#include "Type.hpp"
#include <catch2/catch_all.hpp>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace {
    struct FakeFunDeclParser:public BaseParser<SharedFunDecl>{
        int calledTimes=0;
        SharedFunDecl decl;
        FakeFunDeclParser():BaseParser(nullptr,nullptr){}

        SharedFunDecl parse()override{
            calledTimes++;
            return decl;
        }
    };

    struct FakeStmListParser:public BaseParser<SharedStmList>{
        int calledTimes=0;
        SharedStmList stmList;
        FakeStmListParser():BaseParser(nullptr,nullptr){}

        SharedStmList parse()override{
            calledTimes++;
            return stmList;
        }
    };
}

TEST_CASE("FunParser tests","[FunParserTest.cpp]"){
    auto params=std::make_shared<std::vector<SharedFunParam>>(
        std::vector<SharedFunParam>({
            std::make_shared<FunParam>(
                std::make_shared<std::wstring>(L"رقم1"),
                Type::FLOAT
            ),
            std::make_shared<FunParam>(
                std::make_shared<std::wstring>(L"رقم2"),
                Type::DOUBLE
            )
        })
    );

    auto fakeFunDeclParser=std::make_shared<FakeFunDeclParser>();
    fakeFunDeclParser->decl=std::make_shared<FunDecl>(
        std::make_shared<std::wstring>(L"اسم_الدالة"),
        Type::INT,
        std::make_shared<bool>(false),
        params
    );
    auto fakeFunDeclParserProvider=[&](SharedTokensIterator,SharedBaseScope){
        return fakeFunDeclParser;
    };

    auto fakeStmListParser=std::make_shared<FakeStmListParser>();
    SharedStmListScope passedFunScopeFromStmListProvider;
    auto fakeStmListParserProvider=[&](SharedTokensIterator,SharedStmListScope funScope){
        passedFunScopeFromStmListProvider=funScope;
        return fakeStmListParser;
    }; 

    auto funScope=FunParser(
        getTokensIterator({}), // to be able to access lineNumber property in TokensIterator
        nullptr,
        fakeFunDeclParserProvider,
        fakeStmListParserProvider
    ).parse();
    SECTION("parser should pass funScope to stmListParser"){
        REQUIRE(passedFunScopeFromStmListProvider==funScope);
    }

    SECTION("parameters are added to locals of fun scope"){
        auto locals=*funScope->getLocals();
        REQUIRE(locals.size()==params->size());
        REQUIRE(locals[*params->at(0)->name]->getName()==params->at(0)->name);
        REQUIRE(locals[*params->at(0)->name]->getType()==params->at(0)->type);
        REQUIRE(locals[*params->at(1)->name]->getName()==params->at(1)->name);
        REQUIRE(locals[*params->at(1)->name]->getType()==params->at(1)->type);
    }

    SECTION("return type of fun scope is same as fun decl"){
        REQUIRE(funScope->getReturnType()==fakeFunDeclParser->decl->returnType);
    }

}