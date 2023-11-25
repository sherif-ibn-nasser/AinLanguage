#include "UnitValue.hpp"

UnitValue::UnitValue():IValue(Type::UNIT){}

std::wstring UnitValue::toString(){return *Type::UNIT->getName();}
