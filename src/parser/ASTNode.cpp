#include "ASTNode.hpp"

ASTNode::ASTNode(int lineNumber):lineNumber(lineNumber){}

int ASTNode::getLineNumber()const{
    return lineNumber;
}