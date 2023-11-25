#pragma once
#include "SharedPtrTypes.hpp"
#include <memory>
#include <string>
class ArrayClassScope;
class Type{
    private:
        SharedWString name;
        SharedClassScope classScope;
    public:
        class Array;
        Type(SharedWString name,SharedClassScope classScope=nullptr);
        bool operator==(const Type& type)const;
        bool operator!=(const Type& type)const;
        static SharedWString UNIT_NAME;
        static SharedWString INT_NAME;
        static SharedWString UINT_NAME;
        static SharedWString LONG_NAME;
        static SharedWString ULONG_NAME;
        static SharedWString FLOAT_NAME;
        static SharedWString DOUBLE_NAME;
        static SharedWString CHAR_NAME;
        static SharedWString STRING_NAME;
        static SharedWString BOOL_NAME;
        static SharedWString ARRAY_NAME;
        static SharedType UNIT;
        static SharedType CHAR;
        static SharedType INT;
        static SharedType LONG;
        static SharedType FLOAT;
        static SharedType UINT;
        static SharedType ULONG;
        static SharedType DOUBLE;
        static SharedType BOOL;
        static SharedType STRING;
        static std::shared_ptr<ArrayClassScope> ARRAY_CLASS;

        virtual SharedClassScope getClassScope();
        
        void setClassScope(SharedClassScope classScope);

        SharedWString getName();

        static void addBuiltInClassesTo(SharedFileScope fileScope);

        virtual ~Type();

        const Array* asArray()const;
    
};

class Type::Array:public Type{
    private:
        SharedType type;
    public:
        Array(SharedType type);
        SharedType getType()const;
        SharedClassScope getClassScope()override;
};