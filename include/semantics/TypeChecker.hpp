#pragma once

#include "SharedPtrTypes.hpp"
#include <functional>
#include <memory>
#include <vector>

namespace Semantics{


    class ITypeChecker{
        public:
            virtual void subscribe(int lineNumber,SharedBaseScope searchScope,SharedType type)=0;
            virtual void check()=0;
    };
    
    class TypeChecker:public ITypeChecker{
        private:
            struct TypeCheckStatement{
                int lineNumber;
                SharedBaseScope searchScope;
                SharedType type;

                TypeCheckStatement(
                    int lineNumber,
                    SharedBaseScope searchScope,
                    SharedType type
                );
            };
            std::vector<TypeCheckStatement> checkStatements;
            static inline std::shared_ptr<TypeChecker> instance;
            TypeChecker();
        public:
            static std::shared_ptr<TypeChecker> getInstance();
            void subscribe(int lineNumber,SharedBaseScope searchScope,SharedType type)override;
            void check()override;
    };
};