#include<iostream>
#include "ainio.hpp"
#include "wchar_t_helper.hpp"
#include "ContainsKufrOrUnsupportedCharacterException.hpp"

void ainprint(std::wstring s,bool endl){
    std::ios_base::sync_with_stdio(false);
    std::locale myloc("en_US.UTF-8");                                           
    std::wcout.imbue(myloc);
    std::wcout<<s;
    if(endl)
        std::wcout<<L"\n";
}

std::wstring ainread(bool readline){
    std::ios_base::sync_with_stdio(false);
    std::locale myloc("en_US.UTF-8");
    std::wcin.imbue(myloc);
    std::wstring input;
    if(readline)
        getline(std::wcin,input);
    else
        std::wcin >> input;
    
    for(auto &c:input){
        if(isKufrOrUnsupportedCharacter(c))
            // TODO: show line number
            throw ContainsKufrOrUnsupportedCharacterException(-1,input);
    }
    return input;
}
