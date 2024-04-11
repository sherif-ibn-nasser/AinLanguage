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
    inline auto PLUS_ASSIGN_NAME=L"تخصيص_زائد";
    inline auto MINUS_ASSIGN_NAME=L"تخصيص_ناقص";
    inline auto TIMES_ASSIGN_NAME=L"تخصيص_ضرب";
    inline auto DIV_ASSIGN_NAME=L"تخصيص_قسمة";
    inline auto MOD_ASSIGN_NAME=L"تخصيص_باقي";
    inline auto POW_ASSIGN_NAME=L"تخصيص_أس";
    inline auto SHR_ASSIGN_NAME=L"تخصيص_زح_يمين";
    inline auto SHL_ASSIGN_NAME=L"تخصيص_زح_يسار";
    inline auto BIT_AND_ASSIGN_NAME=L"تخصيص_و";
    inline auto XOR_ASSIGN_NAME=L"تخصيص_عدم_تكافؤ";
    inline auto BIT_OR_ASSIGN_NAME=L"تخصيص_أو";

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
            // Augmented operators
            PLUS_ASSIGN_NAME,MINUS_ASSIGN_NAME,
            TIMES_ASSIGN_NAME,DIV_ASSIGN_NAME,
            MOD_ASSIGN_NAME,POW_ASSIGN_NAME,
            SHR_ASSIGN_NAME,SHL_ASSIGN_NAME,
            BIT_AND_ASSIGN_NAME,XOR_ASSIGN_NAME,BIT_OR_ASSIGN_NAME,
        };

        for(auto& op:OPERATORS_NAMES){
            if(name==op)
                return true;
        }

        return false;
    }
    
}