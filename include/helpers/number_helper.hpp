#pragma once
#include <iostream>
#include <string>
#include <functional>

/*
FIXME: can't differentiate between ints and doubles
*/

bool tryconvert(std::function<void()> convertfun);

bool isint(std::wstring s);

bool isfloat(std::wstring s);

bool isdouble(std::wstring s);

bool islong(std::wstring s);

bool isnum(std::wstring s);
