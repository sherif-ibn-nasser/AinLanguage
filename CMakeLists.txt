cmake_minimum_required(VERSION 3.26.3)

project(AinLanguage)

### start files_cmake.cpp ###

add_library(ainsrc
    src/main.cpp
    src/interpreter/VarsOffsetSetter.cpp
    src/interpreter/Interpreter.cpp
    src/ain_file/AinFile.cpp
    src/lexer/LexerLine.cpp
    src/lexer/Lexer.cpp
    src/lexer/NumberToken.cpp
    src/lexer/LexerToken.cpp
    src/lexer/LiteralToken.cpp
    src/lexer/SymbolToken.cpp
    src/lexer/KeywordToken.cpp
    src/helpers/wchar_t_helper.cpp
    src/helpers/string_helper.cpp
    src/helpers/ainio.cpp
    src/statement/IfStatement.cpp
    src/statement/ContinueStatement.cpp
    src/statement/AssignStatement.cpp
    src/statement/VarStm.cpp
    src/statement/IStatement.cpp
    src/statement/WhileStatement.cpp
    src/statement/BreakStatement.cpp
    src/statement/AugmentedAssignStatement.cpp
    src/statement/ReturnStatement.cpp
    src/statement/ExpressionStatement.cpp
    src/scope/ArrayClassScope.cpp
    src/scope/PackageScope.cpp
    src/scope/StmListScope.cpp
    src/scope/FunScope.cpp
    src/scope/ClassScope.cpp
    src/scope/function/BuiltInFunScope.cpp
    src/scope/Variable.cpp
    src/scope/FileScope.cpp
    src/scope/LoopScope.cpp
    src/scope/StringClassScope.cpp
    src/scope/BaseScope.cpp
    src/semantics/SemanticsChecksVisitor.cpp
    src/semantics/TypeChecker.cpp
    src/semantics/ImplicitVarTypeChecker.cpp
    src/compiler/CompilerVarsOffsetSetter.cpp
    src/compiler/Compiler.cpp
    src/compiler/Assembler.cpp
    src/expression/OperatorFunInvokeExpression.cpp
    src/expression/NewObjectExpression.cpp
    src/expression/value/LongValue.cpp
    src/expression/value/ByteValue.cpp
    src/expression/value/FloatValue.cpp
    src/expression/value/IValue.cpp
    src/expression/value/IntValue.cpp
    src/expression/value/UnitValue.cpp
    src/expression/value/DoubleValue.cpp
    src/expression/value/UByteValue.cpp
    src/expression/value/CharValue.cpp
    src/expression/value/RefValue.cpp
    src/expression/value/BoolValue.cpp
    src/expression/value/StringValue.cpp
    src/expression/value/UIntValue.cpp
    src/expression/value/ULongValue.cpp
    src/expression/NewArrayExpression.cpp
    src/expression/ThisExpression.cpp
    src/expression/VarAccessExpression.cpp
    src/expression/FunInvokeExpression.cpp
    src/expression/NonStaticVarAccessExpression.cpp
    src/expression/IExpression.cpp
    src/expression/LogicalExpression.cpp
    src/expression/NonStaticFunInvokeExpression.cpp
    src/expression/UnitExpression.cpp
    src/expression/ThisFunInvokeExpression.cpp
    src/expression/ThisVarAccessExpression.cpp
    src/expression/SetOperatorExpression.cpp
    src/expression/LiteralExpression.cpp
    src/parser/elements/FunParam.cpp
    src/parser/elements/FunDecl.cpp
    src/parser/elements/Type.cpp
    src/parser/elements/parser/TypeParser.cpp
    src/parser/elements/parser/FunDeclParser.cpp
    src/parser/elements/parser/VarDeclParser.cpp
    src/parser/elements/parser/FunParamParser.cpp
    src/parser/elements/VarDecl.cpp
    src/parser/ClassParser.cpp
    src/parser/ASTNode.cpp
    src/parser/FunParser.cpp
    src/parser/StmListParser.cpp
    src/parser/TokensIterator.cpp
    src/parser/FileParser.cpp
    src/parser/VarStatementParser.cpp
    src/parser/ExpressionParser.cpp
    src/parser/ASTVisitor.cpp
    src/parser/PackageParser.cpp
)

set(INCLUDE
    include/
    include/interpreter/
    include/ain_file/
    include/lexer/
    include/helpers/
    include/exceptions/
    include/exceptions/lexer/
    include/exceptions/statement/
    include/exceptions/semantics/
    include/exceptions/runtime/
    include/exceptions/expression/
    include/exceptions/parser/
    include/statement/
    include/scope/
    include/scope/class/
    include/scope/function/
    include/semantics/
    include/compiler/
    include/expression/
    include/expression/value/
    include/parser/
    include/parser/elements/
    include/parser/elements/parser/
    include/parser/AST/
)

set(TEST
    test/ain_file/AinFileTest.cpp
    test/lexer/LexerTest.cpp
    test/lexer/LexerLineTest.cpp
    test/parser/elements/TypeTest.cpp
    test/parser/elements/FunDeclTest.cpp
    test/parser/elements/VarDeclTest.cpp
    test/parser/elements/FunParamTest.cpp
    test/parser/elements/parser/FunParamParserTest.cpp
    test/parser/elements/parser/TypeParserTest.cpp
    test/parser/elements/parser/FunDeclParserTest.cpp
    test/parser/elements/parser/VarDeclParserTest.cpp
    test/parser/FileParserTest.cpp
    test/parser/FunParserTest.cpp
    test/parser/TokensIteratorForTests.cpp
    test/parser/PackageParserTest.cpp
    test/parser/TokensIteratorTest.cpp
)

### end files_cmake.cpp ###



set(TEST_INCLUDE
    testInclude/
)


add_executable(ain src/main.cpp)

target_include_directories(ainsrc PUBLIC ${INCLUDE})

target_link_libraries(ain ainsrc)

# Add Catch2 library
find_package(Catch2 REQUIRED)

# Add test executable
add_executable(unit_tests ${TEST})

target_include_directories(unit_tests PUBLIC ${TEST_INCLUDE})

# Link Catch2 library to the test executable
target_link_libraries(unit_tests Catch2::Catch2WithMain ainsrc)

# Add test executable
add_executable(pretty_print ExpressionPrettyPrintTest.cpp)

# Link ainsrc library to the test executable
target_link_libraries(pretty_print ainsrc)

# Make executables in build/bin
set_target_properties(ain PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin/)

set_target_properties(unit_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin/)

set_target_properties(pretty_print PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin/)