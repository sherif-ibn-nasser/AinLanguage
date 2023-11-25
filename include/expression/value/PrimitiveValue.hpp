#pragma once
#include "IValue.hpp"
#include "PrimitiveClassScope.hpp"
#include "Type.hpp"
#include <memory>
#include <string>
template<typename T>
class PrimitiveValue:public virtual IValue{
    protected:
        T value;
        std::shared_ptr<PrimitiveClassScope<T>> classScope;
    public:
        PrimitiveValue<T>(SharedType type,T value);
        T getValue();
};

template<typename T>
PrimitiveValue<T>::PrimitiveValue(SharedType type,T value)
:IValue(type),value(value){}

template<typename T>
T PrimitiveValue<T>::getValue(){
    return this->value;
}