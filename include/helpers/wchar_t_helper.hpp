#pragma once

#include <vector>
bool isAinPunct(const wchar_t &c);

bool isAinAlpha(const wchar_t &c);

bool iswempty(const wchar_t &c);

bool isExponentOperator(const wchar_t &c);

bool iswbdigit(const wchar_t &c);

bool iswodigit(const wchar_t &c);

bool isNumSystemChar(const wchar_t &c);

bool isKufrOrUnsupportedCharacter(const wchar_t &c);

    
inline std::vector<wchar_t> kufrAndInvalidChars={
    L'\u03EE',L'\u03EF',L'\u058d',L'\u058e',
    L'\u05EF', // yod triangle
    L'\u07D9',L'\u093B',L'\u13D0',L'\u16BE',L'\u165C',L'\u16ED',
    L'\u17D2',L'\u1D7B',L'\u2020',L'\u2021',L'\u256A',L'\u256B',
    L'\u256C',L'\u2616',L'\u2617',L'\u269C',L'\u269E',L'\u269F',
    L'\u26AF',L'\u26B0',L'\u26B1',L'\u26F3',L'\u26F9',L'\u26FB',
    L'\u26FF',L'\u27CA',L'\u29FE',L'\u2CFE',
};

inline std::vector<std::pair<wchar_t,wchar_t>> kufrAndInvalidCharsRanges={
    /*  from  ,    to  */
    {L'\u0900',L'\u109F'},//HinduEurope
    {L'\u1100',L'\u1C7F'},//HinduEurope
    {L'\u253C',L'\u254B'},
    {L'\u2624',L'\u2638'},//Kufr
    {L'\u263D',L'\u2653'},//Kufr
    {L'\u2654',L'\u2667'},
    {L'\u2669',L'\u2671'},//Music and kufr crosses
    {L'\u2680',L'\u268F'},
    {L'\u2680',L'\u268F'},
    {L'\u26A2',L'\u26A9'},// Pride
    {L'\u26B3',L'\u26BC'},// Kufr
    {L'\u26BF',L'\u26EC'},
    {L'\u2719',L'\u2725'},// Kufr crosses
    {L'\u2BF0',L'\u2C5F'},// Includes astrology
    {L'\u2D80',L'\uAB2F'},
    {L'\uAB70',L'\uFAFF'},
};