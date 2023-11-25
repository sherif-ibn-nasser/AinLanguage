#pragma once
#include "BoolValue.hpp"
#include "IExpression.hpp"
#include "SharedPtrTypes.hpp"
#include <functional>
#include <memory>

class LogicalExpression:public IExpression{
    protected:
        std::shared_ptr<BoolValue> evaluateRight();
    public:
        enum class Operation{
            OR,AND
        };
        LogicalExpression(
            int lineNumber,
            Operation logicalOp,
            SharedIExpression left,
            SharedIExpression right
        );
        void accept(ASTVisitor *visitor) override;
        
        SharedIExpression getLeft() const;

        SharedIExpression getRight() const;

        Operation getLogicalOp() const;
        
    private:
        SharedIExpression left;
        SharedIExpression right;
        Operation logicalOp;
};