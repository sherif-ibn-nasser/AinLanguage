#include "ThisExpression.hpp"
#include "SharedPtrTypes.hpp"
#include "ClassScope.hpp"
#include "Type.hpp"
#include <memory>
#include <string>

ThisExpression::ThisExpression(int lineNumber,SharedClassScope classScope):
    IExpression(
        lineNumber,
        std::make_shared<Type>(
            std::make_shared<std::wstring>(classScope->getName()),
            classScope
        )
    )
{}