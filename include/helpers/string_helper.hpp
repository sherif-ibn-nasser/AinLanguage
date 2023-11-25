#pragma once
#include <string>
bool startsWith(std::string str, std::string prefix);

bool endsWith(std::string str, std::string suffix);

void removeUnderscores(std::wstring* wstr);

/**
 * @brief Get the unicode character from [codePoint]
 * @returns unicode wchar_t 
 * @throws std::invalid_argument @if invalid codePoint
*/
wchar_t getUnicodeCharacterFromCode(std::wstring codePoint);

/**
 * @brief Get the escape sequence character from [c]
 * @if [c] for example is L'n', this method will L'\n'
 * @else @throws std::invalid_argument
*/
wchar_t getEscapeSequenceFromCharacter(const wchar_t& c);

const char* toCharPointer(const std::wstring& wstr);

std::wstring toWstring(std::string str);