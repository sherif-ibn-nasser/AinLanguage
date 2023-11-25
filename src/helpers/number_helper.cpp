#include "number_helper.hpp"

bool tryconvert(std::function<void()> convertfun){
    try{
        convertfun();
        return true;
    }catch(std::exception e){
        return false;
    }
}

bool isint(std::wstring s){
    int i;
    return tryconvert([&](){
        i=std::stoi(s);
    });
}

bool islong(std::wstring s){
    long l;
    return tryconvert([&](){
        l=std::stol(s);
    });
}

bool isfloat(std::wstring s){
    float f;
    return tryconvert([&](){
        f=std::stof(s);
    });
}

bool isdouble(std::wstring s){
    double d;
    return tryconvert([&](){
        d=std::stod(s);
    });
}

bool isnum(std::wstring s){
    return isint(s)||islong(s)||isfloat(s)||isdouble(s);
}
