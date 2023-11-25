#pragma once
#include "ASTVisitor.hpp"
class ASTNode{
    private:
        int lineNumber;
    public:

        ASTNode(int lineNumber);

        virtual void accept(ASTVisitor* visitor)=0;
        
        virtual ~ASTNode(){}

        int getLineNumber()const;  
};