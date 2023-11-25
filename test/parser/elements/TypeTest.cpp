#include <catch2/catch_all.hpp>
#include <memory>
#include "Type.hpp"

TEST_CASE("Type tests", "[TypeTest.cpp]") {
    // Create some shared pointers to test data

    SECTION("constructor sets name property correctly") {
        // Create a new Type object
        Type intType(Type::INT_NAME);

        // Check that the object's name property was set correctly
        REQUIRE(intType.getName() == Type::INT_NAME);
    }

    SECTION("equality operator returns true for equal objects") {
        // Create two Type objects with identical names
        Type intType1(Type::INT_NAME,Type::INT->getClassScope());
        Type intType2(Type::INT_NAME,Type::INT->getClassScope());

        // Check that the equality operator returns true
        REQUIRE(intType1 == intType2);

        Type::Array a1(std::make_shared<Type>(intType1));
        Type::Array a2(std::make_shared<Type>(intType2));

        // Check that the equality operator returns true on arrays
        REQUIRE(a1 == a2);

        Type::Array aa1(std::make_shared<Type::Array>(a1));
        Type::Array aa2(std::make_shared<Type::Array>(a2));

        // Check that the equality operator returns true on arrays of arrays
        REQUIRE(aa1 == aa2);

        // Check that the equality operator returns false on arrays and non-arrays
        REQUIRE_FALSE(a1 == intType1);

        // Check that the equality operator returns false on arrays and arrays of arrays
        REQUIRE_FALSE(aa1 == a1);
    }

    SECTION("equality operator returns false for objects with different names") {
        // Create two Type objects with different names
        Type intType(Type::INT_NAME,Type::INT->getClassScope());
        Type doubleType(Type::DOUBLE_NAME,Type::DOUBLE->getClassScope());

        // Check that the equality operator returns false
        REQUIRE_FALSE(intType == doubleType);

        Type::Array a1(std::make_shared<Type>(intType));
        Type::Array a2(std::make_shared<Type>(doubleType));

        // Check that the equality operator returns false on arrays
        REQUIRE_FALSE(a1 == a2);

        Type::Array aa1(std::make_shared<Type::Array>(a1));
        Type::Array aa2(std::make_shared<Type::Array>(a2));

        // Check that the equality operator returns false on arrays of arrays
        REQUIRE_FALSE(aa1 == aa2);
    }

    SECTION("inequality operator returns true for unequal objects") {
        // Create two Type objects with different names
        Type intType(Type::INT_NAME,Type::INT->getClassScope());
        Type doubleType(Type::DOUBLE_NAME,Type::DOUBLE->getClassScope());

        // Check that the inequality operator returns true
        REQUIRE(intType != doubleType);

        Type::Array a1(std::make_shared<Type>(intType));
        Type::Array a2(std::make_shared<Type>(doubleType));

        // Check that the inequality operator returns true on arrays
        REQUIRE(a1 != a2);

        Type::Array aa1(std::make_shared<Type::Array>(a1));
        Type::Array aa2(std::make_shared<Type::Array>(a2));

        // Check that the inequality operator returns true on arrays of arrays
        REQUIRE(aa1 != aa2);
    }

    SECTION("inequality operator returns false for equal objects") {
        // Create two Type objects with identical names
        Type intType1(Type::INT_NAME,Type::INT->getClassScope());
        Type intType2(Type::INT_NAME,Type::INT->getClassScope());

        // Check that the inequality operator returns false
        REQUIRE_FALSE(intType1 != intType2);

        Type::Array a1(std::make_shared<Type>(intType1));
        Type::Array a2(std::make_shared<Type>(intType2));

        // Check that the inequality operator returns false on arrays
        REQUIRE_FALSE(a1 != a2);

        Type::Array aa1(std::make_shared<Type::Array>(a1));
        Type::Array aa2(std::make_shared<Type::Array>(a2));

        // Check that the inequality operator returns false on arrays of arrays
        REQUIRE_FALSE(aa1 != aa2);

        // Check that the inequality operator returns true on arrays and non-arrays
        REQUIRE(a1 != intType1);

        // Check that the inequality operator returns true on arrays and arrays of arrays
        REQUIRE(aa1 != a1);
    }
}