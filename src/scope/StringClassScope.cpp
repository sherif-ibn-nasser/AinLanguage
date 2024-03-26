#include "StringClassScope.hpp"
#include "Type.hpp"
#include "Variable.hpp"
#include <string>

StringClassScope::StringClassScope()
:ClassScope(0,*Type::STRING_NAME,PackageScope::AIN_PACKAGE){
    (*getPublicVariables())[*CAPACITY_NAME]=
        std::make_shared<Variable>(
            CAPACITY_NAME,
            Type::ULONG,
            std::make_shared<bool>(true)
        );
    (*getPublicVariables())[*SIZE_NAME]=
        std::make_shared<Variable>(
            SIZE_NAME,
            Type::ULONG,
            std::make_shared<bool>(true)
        );
}