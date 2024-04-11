#include "ArrayClassScope.hpp"

ArrayClassScope::ArrayClassScope():
PrimitiveClassScope<std::vector<SharedIValue>>(Type::ARRAY_NAME){    
    (*getPublicVariables())[*CAPACITY_NAME]=
        std::make_shared<Variable>(
            CAPACITY_NAME,
            Type::ULONG,
            std::make_shared<bool>(true)
        );
}

SharedFunScope ArrayClassScope::GET;

SharedFunScope ArrayClassScope::SET;