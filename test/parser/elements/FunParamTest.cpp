#include <catch2/catch_all.hpp>
#include "FunParam.hpp"
#include "Type.hpp"

TEST_CASE("FunParam tests", "[FunParamTest.cpp]") {
    // Create some shared pointers to test data
    auto paramName = std::make_shared<std::wstring>(L"myParam");
    auto intType = Type::INT;
    auto doubleType=Type::DOUBLE;

    SECTION("constructor sets properties correctly") {
        // Create a new FunParam object
        FunParam myParam(paramName, intType);

        // Check that the object's properties were set correctly
        REQUIRE(myParam.name == paramName);
        REQUIRE(myParam.type == intType);
    }

    SECTION("equality operator returns true for equal objects") {
        // Create two FunParam objects with identical properties
        FunParam myParam1(paramName, intType);
        FunParam myParam2(paramName, intType);

        // Check that the equality operator returns true
        REQUIRE(myParam1 == myParam2);
    }

    SECTION("equality operator returns false for objects with different names") {
        // Create two FunParam objects with different names
        SharedWString paramName1 = std::make_shared<std::wstring>(L"myParam1");
        SharedWString paramName2 = std::make_shared<std::wstring>(L"myParam2");
        FunParam myParam1(paramName1, intType);
        FunParam myParam2(paramName2, intType);

        // Check that the equality operator returns false
        REQUIRE_FALSE(myParam1 == myParam2);
    }

    SECTION("equality operator returns false for objects with different types") {
        // Create two FunParam objects with different types
        FunParam myParam1(paramName, intType);
        FunParam myParam2(paramName, doubleType);

        // Check that the equality operator returns false
        REQUIRE_FALSE(myParam1 == myParam2);
    }

    SECTION("inequality operator returns true for unequal objects") {
        // Create two FunParam objects with different names and types
        SharedWString paramName1 = std::make_shared<std::wstring>(L"myParam1");
        SharedWString paramName2 = std::make_shared<std::wstring>(L"myParam2");
        FunParam myParam1(paramName1, intType);
        FunParam myParam2(paramName2, doubleType);

        // Check that the inequality operator returns true
        REQUIRE(myParam1 != myParam2);
    }

    SECTION("inequality operator returns false for equal objects") {
        // Create two FunParam objects with identical properties
        FunParam myParam1(paramName, intType);
        FunParam myParam2(paramName, intType);

        // Check that the inequality operator returns false
        REQUIRE_FALSE(myParam1 != myParam2);
    }
}