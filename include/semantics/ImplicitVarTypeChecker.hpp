#pragma once
#include "SemanticsChecksVisitor.hpp"
#include "VarStm.hpp"
#include "SharedPtrTypes.hpp"
#include <memory>
#include <vector>

namespace Semantics{

    class ImplicitVarTypeChecker{
        private:
            std::vector<std::shared_ptr<VarStm>> checkStatements;
            static inline std::shared_ptr<ImplicitVarTypeChecker> instance;
            ImplicitVarTypeChecker();
        public:
            static std::shared_ptr<ImplicitVarTypeChecker> getInstance();
            void subscribe(std::shared_ptr<VarStm> varStm);
            void check(SemanticsChecksVisitor* checker);
    };
};