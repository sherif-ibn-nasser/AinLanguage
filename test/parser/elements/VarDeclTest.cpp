#include "VarDecl.hpp"
#include "StringMaker.hpp"

TEST_CASE("VarDecl equality and inequality", "[VarDeclTest.cpp]") {
    SharedWString name1 = std::make_shared<std::wstring>(L"name1");
    SharedWString name2 = std::make_shared<std::wstring>(L"name2");
    SharedType typeInt = Type::INT;
    SharedType typeBool = Type::BOOL;
    SharedBool isValFalse = std::make_shared<bool>(false);
    SharedBool isValTrue = std::make_shared<bool>(true);
    
    VarDecl decl1(name1, typeInt, isValFalse);
    VarDecl decl2(name1, typeInt, isValFalse);
    VarDecl decl3(name2, typeInt, isValFalse);
    VarDecl decl4(name1, typeBool, isValFalse);
    VarDecl decl5(name1, typeInt, isValTrue);
    VarDecl decl6(name1, nullptr, isValFalse);
    
    SECTION("Equal variable declarations") {
        REQUIRE(decl1 == decl2);
    }

    SECTION("Inequality is the inverse of equality") {
        REQUIRE((decl1 != decl2) == !(decl1 == decl2));
    }
    
    SECTION("Variable declarations with different names") {
        REQUIRE_FALSE(decl1 == decl3);
    }
    
    SECTION("Variable declarations with different types") {
        REQUIRE_FALSE(decl1 == decl4);
    }
    
    SECTION("Variable declarations with different values for isVal") {
        REQUIRE_FALSE(decl1 == decl5);
    }
    
    SECTION("test hasImplicitReturnType()"){
        REQUIRE_FALSE(decl1.hasImplicitType());
        REQUIRE(decl6.hasImplicitType());

        SECTION("equality is false if one variable has implicit type or both"){
            REQUIRE_FALSE(decl1 == decl6);
            decl1.type=nullptr;
            REQUIRE_FALSE(decl1 == decl6);
            decl6.type=typeInt;
            REQUIRE_FALSE(decl1 == decl6);
        }        
    }
}