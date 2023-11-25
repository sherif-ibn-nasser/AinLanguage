#include <catch2/catch_all.hpp>
#include "FunDecl.hpp"
#include "FunParam.hpp"
#include "Type.hpp"

TEST_CASE("FunDecl tests", "[FunDeclTest.cpp]") {
    // Create some shared pointers to test data
    auto name = std::make_shared<std::wstring>(L"myFunction");
    auto returnType=Type::BOOL;
    auto isOperator=std::make_shared<bool>(false);
    auto params = std::make_shared<std::vector<SharedFunParam>>();

    SECTION("constructor sets properties correctly") {
        // Create a new FunDecl object
        FunDecl myFunction(name, returnType, isOperator, params);

        // Check that the object's properties were set correctly
        REQUIRE(myFunction.name == name);
        REQUIRE_FALSE(myFunction.hasImplicitReturnType());
        REQUIRE(myFunction.returnType == returnType);
        REQUIRE(myFunction.isOperator == isOperator);
        REQUIRE(myFunction.params == params);
    }

    SECTION("equality operator returns true for equal objects") {
        // Create two FunDecl objects with identical properties
        FunDecl myFunction1(name, returnType, isOperator, params);
        FunDecl myFunction2(name, returnType, isOperator, params);

        // Check that the equality operator returns true
        REQUIRE(myFunction1 == myFunction2);
    }

    SECTION("equality operator returns false when one return type is null or both") {
        // Create two FunDecl objects with identical properties
        FunDecl myFunction1(name, nullptr, isOperator, params);
        FunDecl myFunction2(name, returnType, isOperator, params);

        // Check that the equality operator returns false
        REQUIRE_FALSE(myFunction1 == myFunction2);

        // second fun has implicit type
        myFunction2.returnType=nullptr;
        REQUIRE_FALSE(myFunction1 == myFunction2);


        // first fun has explicit type
        myFunction1.returnType=returnType;
        REQUIRE_FALSE(myFunction1 == myFunction2);
    }

    SECTION("equality operator returns false for objects with different parameters") {
        // Create two FunDecl objects with different parameter lists
        auto params1 = std::make_shared<std::vector<SharedFunParam>>();
        auto params2 = std::make_shared<std::vector<SharedFunParam>>();
        auto name=std::make_shared<std::wstring>(L"param1");
        params1->push_back(std::make_shared<FunParam>(name,Type::INT));
        params2->push_back(std::make_shared<FunParam>(name,Type::DOUBLE));

        FunDecl myFunction1(name, returnType, isOperator, params1);
        FunDecl myFunction2(name, returnType, isOperator, params2);

        // Check that the equality operator returns false
        REQUIRE_FALSE(myFunction1 == myFunction2);
    }

    SECTION("inequality operator returns true for unequal objects") {
        // Create two FunDecl objects with different names
        auto name1 = std::make_shared<std::wstring>(L"myFunction1");
        auto name2 = std::make_shared<std::wstring>(L"myFunction2");
        FunDecl myFunction1(name1, returnType, isOperator, params);
        FunDecl myFunction2(name2, returnType, isOperator, params);

        // Check that the inequality operator returns true
        REQUIRE(myFunction1 != myFunction2);
    }

    SECTION("inequality operator returns false for equal objects") {
        // Create two FunDecl objects with identical properties
        FunDecl myFunction1(name, returnType, isOperator, params);
        FunDecl myFunction2(name, returnType, isOperator, params);

        // Check that the inequality operator returns false
        REQUIRE_FALSE(myFunction1 != myFunction2);
    }
}