#include <iostream>
#include <string>
#include <algorithm>
#include <locale>
#include <codecvt>
#include "string_helper.hpp"
bool startsWith(std::string str, std::string prefix)
{
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

bool endsWith(std::string str, std::string suffix)
{
    return str.size() >= suffix.size() && 0 == str.compare(str.size()-suffix.size(), suffix.size(), suffix);
}

void removeUnderscores(std::wstring* wstr){
    wstr->erase(
        std::remove_if(
            wstr->begin(),
            wstr->end(),
            [](auto c){return c==L'_';}
        ),
        wstr->end()
    );
}

wchar_t getUnicodeCharacterFromCode(std::wstring codePoint){
    auto allDigits=std::all_of(codePoint.begin(), codePoint.end(), ::iswxdigit);
    if(codePoint.size()!=4||!allDigits)
        std::__throw_invalid_argument("Invalid universal charcater code.");
    auto uniCharInt=std::stoi(codePoint,0,16);
    auto uniChar=static_cast<wchar_t>(uniCharInt);
    return uniChar;
}

wchar_t getEscapeSequenceFromCharacter(const wchar_t& c){
    switch(c){
        // مسافة للخلف
        case L'خ' :return L'\b';

        // مساحة أفقية
        case L'ف' :return L'\t';

        // سطر جديد
        case L'س' :return L'\n';

        // مساحة رأسية
        case L'ر' :return L'\v';

        // الصفحة التالية
        case L'ص' :return L'\f';

        // إرجاع المؤشر إلى بداية السطر، وبدء الكتابة منه
        case L'ج' :return L'\r';
        
        case L'\\':return L'\\';
        case L'\"':return L'\"';
        case L'\'':return L'\'';
        default:
            std::__throw_invalid_argument("Invalid escape sequence.");
    }
}

const char* toCharPointer(const std::wstring& wstr) {
    std::setlocale(0,"en_US.UTF-8");
        
    auto wmsg=wstr.c_str();
    
    // Get the length of the multibyte string
    std::size_t len = std::wcstombs(nullptr, wmsg, 0);

    // Allocate a buffer to hold the multibyte string
    char* mbstr = new char[len + 1];

    // Convert the wide character string to a multibyte character string
    std::wcstombs(mbstr, wmsg, len + 1);

    return mbstr;
}

std::wstring toWstring(std::string str){
    // Create a wide string using the codecvt_utf8_utf16 facet
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}