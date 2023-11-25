#pragma once
#include "IExpression.hpp"
#include "SharedPtrTypes.hpp"
#include <string>
#include <vector>
class NewArrayExpression:public IExpression{
    private:
        // The capacity expressions for a multi-dimensional array
        std::vector<SharedIExpression>capacities;
    public:
        NewArrayExpression(
            int lineNumber,
            std::vector<SharedIExpression>capacities,
            SharedType type
        );
        void accept(ASTVisitor *visitor) override;
        std::vector<SharedIExpression> getCapacities() const;
};