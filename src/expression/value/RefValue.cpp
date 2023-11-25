#include "RefValue.hpp"
#include "Type.hpp"

std::wstring RefValue::toString(){
    return L"";
}

RefValue::RefValue(int value)
    : IValue(Type::INT),
    address(value){}
    
int RefValue::getAddress()const{
    return address;
}