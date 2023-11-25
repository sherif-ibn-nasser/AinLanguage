#pragma once
#include "LexerToken.hpp"
#include "SymbolToken.hpp"
#include <string>

namespace OperatorFunctions{

    inline auto PLUS_NAME=L"زائد";
    inline auto MINUS_NAME=L"ناقص";
    inline auto TIMES_NAME=L"ضرب";
    inline auto DIV_NAME=L"قسمة";
    inline auto MOD_NAME=L"باقي";
    inline auto POW_NAME=L"أس";
    inline auto COMPARE_TO_NAME=L"قارن_مع";
    inline auto EQUALS_NAME=L"يساوي";
    inline auto SHR_NAME=L"زح_يمين";
    inline auto SHL_NAME=L"زح_يسار";
    inline auto BIT_AND_NAME=L"و";
    inline auto XOR_NAME=L"عدم_تكافؤ";
    inline auto BIT_OR_NAME=L"أو";
    inline auto LOGICAL_NOT_NAME=L"نفي";
    inline auto BIT_NOT_NAME=L"نفي_بت";
    inline auto UNARY_PLUS_NAME=L"موجب";
    inline auto UNARY_MINUS_NAME=L"سالب";
    inline auto INC_NAME=L"زد";
    inline auto DEC_NAME=L"أنقص";
    inline auto GET_NAME=L"جلب";
    inline auto SET_NAME=L"تعيين";

    inline bool isOperatorFunName(std::wstring name){
        
        auto OPERATORS_NAMES={
            // Binary operators
            PLUS_NAME,MINUS_NAME,
            TIMES_NAME,DIV_NAME,
            MOD_NAME,POW_NAME,
            COMPARE_TO_NAME,EQUALS_NAME,
            SHR_NAME,SHL_NAME,
            BIT_AND_NAME,XOR_NAME,BIT_OR_NAME,
            // Unary operators
            UNARY_PLUS_NAME,UNARY_MINUS_NAME,
            LOGICAL_NOT_NAME,BIT_NOT_NAME,
            INC_NAME,DEC_NAME,
            GET_NAME,SET_NAME,
        };

        for(auto& op:OPERATORS_NAMES){
            if(name==op)
                return true;
        }

        return false;
    }
    
}