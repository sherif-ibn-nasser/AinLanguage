#pragma once
#include "BaseParser.hpp"
#include "SharedPtrTypes.hpp"
#include "TypeChecker.hpp"
#include <memory>
class TypeParser:public BaseParser<SharedType>{
    private:
        SharedITypeChecker typeChecker;
    public:
        TypeParser(
            SharedTokensIterator iterator,
            SharedBaseScope scope,
            SharedITypeChecker typeChecker
        );
        SharedType parse()override;
};