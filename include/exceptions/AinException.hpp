#pragma once
#include "string_helper.hpp"
#include<iostream>
#include<algorithm>
#include<string>
#include <cstdlib>
#include <cwchar>

class AinException:public std::exception{
    private:
    std::wstring msgStr;

    public:

    AinException(std::wstring msgStr):msgStr(msgStr){}

    const char* what() const noexcept override{
        return toCharPointer(msgStr);
    }

    std::wstring whatWstr(){
        return msgStr;
    }

    static std::wstring errorWString(std::wstring wstr){
        return L"\033[1;31m"+wstr+L"\033[0m";
    }

    static std::wstring betweenAngleBrackets(std::wstring wstr){
        return L"<"+wstr+L">";
    }
    
    static std::wstring removeNullChar(std::wstring wstr){
        wstr.erase(remove(wstr.begin(), wstr.end(), L'\0'), wstr.end());
        return wstr;
    }
};