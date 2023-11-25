#include "NewArrayExpression.hpp"
#include "Type.hpp"
#include <memory>

NewArrayExpression::NewArrayExpression(
    int lineNumber,
    std::vector<SharedIExpression>capacities,
    SharedType type
):IExpression(lineNumber,nullptr),capacities(capacities){

    auto initReturnType=type;

    for(auto c:capacities){
        initReturnType=std::make_shared<Type::Array>(initReturnType);
    }

    this->returnType=initReturnType;
}

std::vector<SharedIExpression> NewArrayExpression::getCapacities()const{
    return this->capacities;
}