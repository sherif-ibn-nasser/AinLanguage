#include "VoidValue.hpp"

VoidValue::VoidValue():IValue(Type::VOID){}

std::wstring VoidValue::toString(){return *Type::VOID->getName();}
