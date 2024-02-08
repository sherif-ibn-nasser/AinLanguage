#include "BuiltInFunScope.hpp"
#include "ArrayIndexOutOfRangeException.hpp"
#include "BoolClassScope.hpp"
#include "BoolValue.hpp"
#include "CharClassScope.hpp"
#include "CharValue.hpp"
#include "ContainsKufrOrUnsupportedCharacterException.hpp"
#include "DoubleClassScope.hpp"
#include "DoubleValue.hpp"
#include "FloatClassScope.hpp"
#include "FloatValue.hpp"
#include "FunParam.hpp"
#include "IntClassScope.hpp"
#include "IntValue.hpp"
#include "KeywordToken.hpp"
#include "LongClassScope.hpp"
#include "LongValue.hpp"
#include "OperatorFunctions.hpp"
#include "PackageScope.hpp"
#include "FunDecl.hpp"
#include "RefValue.hpp"
#include "SharedPtrTypes.hpp"
#include "StringClassScope.hpp"
#include "StringValue.hpp"
#include "Type.hpp"
#include "UIntClassScope.hpp"
#include "UIntValue.hpp"
#include "ULongClassScope.hpp"
#include "ULongValue.hpp"
#include "UnitValue.hpp"
#include "ArrayClassScope.hpp"
#include "Variable.hpp"
#include "ainio.hpp"
#include "FileScope.hpp"
#include "runtime/NumberFormatException.hpp"
#include "wchar_t_helper.hpp"
#include <exception>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

BuiltInFunScope::BuiltInFunScope(
    std::wstring name,
    SharedType returnType,
    std::vector<std::pair<std::wstring, SharedType>>params,
    std::function<void(Interpreter*)> invokeOnInterpreterFun,
    bool isOperator,
    std::function<std::wstring()> onGenerateAsm
):
    FunScope(
        0,
        PackageScope::AIN_PACKAGE,
        std::make_shared<FunDecl>(
            std::make_shared<std::wstring>(name),
            returnType,
            std::make_shared<bool>(isOperator),
            std::make_shared<std::vector<SharedFunParam>>()
        )
    ),
    invokeOnInterpreterFun(invokeOnInterpreterFun),
    onGenerateAsm(onGenerateAsm)
{
    for(auto paramsIterator:params){
        auto name=std::make_shared<std::wstring>(paramsIterator.first);
        auto type=paramsIterator.second;
        auto isVal=std::make_shared<bool>(true);
        decl->params->push_back(
            std::make_shared<FunParam>(name,type)
        );
        (*locals)[*name]=std::make_shared<Variable>(name,type,isVal);
    }
}

BuiltInFunScope::~BuiltInFunScope(){}

void BuiltInFunScope::invokeOnInterpreter(Interpreter* interpreter){
    invokeOnInterpreterFun(interpreter);
}

std::wstring BuiltInFunScope::getGeneratedAsm(){
    return onGenerateAsm();
}

void BuiltInFunScope::addBuiltInFunctionsTo(SharedFileScope fileScope){

    auto SYSCALL0=std::make_shared<BuiltInFunScope>(
        SYSCALL_NAME,
        Type::LONG,
        std::vector<std::pair<std::wstring, SharedType>>{
            {L"_RAX_",Type::LONG}, // rax
        },
        [](Interpreter* interpreter){},
        false,
        [](){
            return
                L"\tpop RAX\n"
                L"\tsyscall\n"
            ;
        }
    );

    auto SYSCALL1=std::make_shared<BuiltInFunScope>(
        SYSCALL_NAME,
        Type::LONG,
        std::vector<std::pair<std::wstring, SharedType>>{
            {L"_RAX_",Type::LONG}, // rax
            {L"_RDI_",Type::LONG}, // rdi
        },
        [](Interpreter* interpreter){},
        false,
        [](){
            return
                L"\tpop RDI\n"
                L"\tpop RAX\n"
                L"\tsyscall\n"
            ;
        }
    );

    auto SYSCALL2=std::make_shared<BuiltInFunScope>(
        SYSCALL_NAME,
        Type::LONG,
        std::vector<std::pair<std::wstring, SharedType>>{
            {L"_RAX_",Type::LONG}, // rax
            {L"_RDI_",Type::LONG}, // rdi
            {L"_RSI_",Type::LONG}, // rsi
        },
        [](Interpreter* interpreter){},
        false,
        [](){
            return
                L"\tpop RSI\n"
                L"\tpop RDI\n"
                L"\tpop RAX\n"
                L"\tsyscall\n"
            ;
        }
    );

    auto SYSCALL3=std::make_shared<BuiltInFunScope>(
        SYSCALL_NAME,
        Type::LONG,
        std::vector<std::pair<std::wstring, SharedType>>{
            {L"_RAX_",Type::LONG}, // rax
            {L"_RDI_",Type::LONG}, // rdi
            {L"_RSI_",Type::LONG}, // rsi
            {L"_RDX_",Type::LONG}, // rdx
        },
        [](Interpreter* interpreter){},
        false,
        [](){
            return
                L"\tpop RDX\n"
                L"\tpop RSI\n"
                L"\tpop RDI\n"
                L"\tpop RAX\n"
                L"\tsyscall\n"
            ;
        }
    );

    auto SYSCALL4=std::make_shared<BuiltInFunScope>(
        SYSCALL_NAME,
        Type::LONG,
        std::vector<std::pair<std::wstring, SharedType>>{
            {L"_RAX_",Type::LONG}, // rax
            {L"_RDI_",Type::LONG}, // rdi
            {L"_RSI_",Type::LONG}, // rsi
            {L"_RDX_",Type::LONG}, // rdx
            {L"_R10_",Type::LONG}, // r10
        },
        [](Interpreter* interpreter){},
        false,
        [](){
            return
                L"\tpop R10\n"
                L"\tpop RDX\n"
                L"\tpop RSI\n"
                L"\tpop RDI\n"
                L"\tpop RAX\n"
                L"\tsyscall\n"
            ;
        }
    );

    auto SYSCALL5=std::make_shared<BuiltInFunScope>(
        SYSCALL_NAME,
        Type::LONG,
        std::vector<std::pair<std::wstring, SharedType>>{
            {L"_RAX_",Type::LONG}, // rax
            {L"_RDI_",Type::LONG}, // rdi
            {L"_RSI_",Type::LONG}, // rsi
            {L"_RDX_",Type::LONG}, // rdx
            {L"_R10_",Type::LONG}, // r10
            {L"_R8_",Type::LONG} , // r8
        },
        [](Interpreter* interpreter){},
        false,
        [](){
            return
                L"\tpop R8\n"
                L"\tpop R10\n"
                L"\tpop RDX\n"
                L"\tpop RSI\n"
                L"\tpop RDI\n"
                L"\tpop RAX\n"
                L"\tsyscall\n"
            ;
        }
    );

    auto SYSCALL6=std::make_shared<BuiltInFunScope>(
        SYSCALL_NAME,
        Type::LONG,
        std::vector<std::pair<std::wstring, SharedType>>{
            {L"_RAX_",Type::LONG}, // rax
            {L"_RDI_",Type::LONG}, // rdi
            {L"_RSI_",Type::LONG}, // rsi
            {L"_RDX_",Type::LONG}, // rdx
            {L"_R10_",Type::LONG}, // r10
            {L"_R8_",Type::LONG} , // r8
            {L"_R9_",Type::LONG} , // r9
        },
        [](Interpreter* interpreter){},
        false,
        [](){
            return
                L"\tpop R9\n"
                L"\tpop R8\n"
                L"\tpop R10\n"
                L"\tpop RDX\n"
                L"\tpop RSI\n"
                L"\tpop RDI\n"
                L"\tpop RAX\n"
                L"\tsyscall\n"
            ;
        }
    );

    auto BRK=std::make_shared<BuiltInFunScope>(
        BRK_NAME,
        Type::LONG,
        std::vector<std::pair<std::wstring, SharedType>>{
            {L"الإزاحة",Type::LONG},
        },
        [](Interpreter* interpreter){},
        false,
        [](){
            return
                L"\tpop RDI\n"
                L"\tmov RSI, [brk_end]\n"
                L"\tlea RDI, [RDI+RSI]\n"
                L"\tmov RAX, 12\n"
                L"\tsyscall\n"
                L"\tmov [brk_end], RAX\n"
            ;
        }
    );

    auto WRITE_LONG_TO_ADDRESS=std::make_shared<BuiltInFunScope>(
        WRITE_TO_ADDRESS_NAME,
        Type::LONG,
        std::vector<std::pair<std::wstring, SharedType>>{
            {ADDRESS_PARAM_NAME,Type::LONG},
            {LONG_PARAM_NAME,Type::LONG},
        },
        [](Interpreter* interpreter){},
        false,
        [](){
            return
                L"\tpop RDI\n"
                L"\tpop RAX\n"
                L"\tmov [RAX], RDI\n"
                L"\txor RAX, RAX\n" // It returns 0 after a successful write
            ;
        }
    );

    auto READ_LONG_FROM_ADDRESS=std::make_shared<BuiltInFunScope>(
        READ_LONG_FROM_ADDRESS_NAME,
        Type::LONG,
        std::vector<std::pair<std::wstring, SharedType>>{
            {ADDRESS_PARAM_NAME,Type::LONG},
        },
        [](Interpreter* interpreter){},
        false,
        [](){
            return
                L"\tpop RAX\n"
                L"\tmov RAX, [RAX]\n"
            ;
        }
    );

    auto READ=std::make_shared<BuiltInFunScope>(
        READ_NAME,
        Type::STRING,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){
            auto input=ainread(false);
            interpreter->AX=std::make_shared<StringValue>(input);
        }
    );

    auto READ_LINE=std::make_shared<BuiltInFunScope>(
        READ_LINE_NAME,
        Type::STRING,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){
            auto input=ainread(true);
            interpreter->AX=std::make_shared<StringValue>(input);
        }
    );

    auto PRINT_INVOKE_INTERPRETER_FUN=
    [](Interpreter* interpreter){
        auto msg=interpreter->top();
        ainprint(msg->toString(), false);
        interpreter->AX=std::make_shared<UnitValue>();
    };

    auto PRINTLN_INVOKE_INTERPRETER_FUN=
    [](Interpreter* interpreter){
        auto msg=interpreter->top();
        ainprint(msg->toString(), true);
        interpreter->AX=std::make_shared<UnitValue>();
    };

    auto PRINT_INT=std::make_shared<BuiltInFunScope>(
        PRINT_NAME,
        Type::UNIT,
        std::vector<std::pair<std::wstring, SharedType>>{{INT_PARAM_NAME,Type::INT}},
        PRINT_INVOKE_INTERPRETER_FUN
    );

    auto PRINTLN_INT=std::make_shared<BuiltInFunScope>(
        PRINTLN_NAME,
        Type::UNIT,
        std::vector<std::pair<std::wstring, SharedType>>{{INT_PARAM_NAME,Type::INT}},
        PRINTLN_INVOKE_INTERPRETER_FUN
    );

    auto PRINT_UINT=std::make_shared<BuiltInFunScope>(
        PRINT_NAME,
        Type::UNIT,
        std::vector<std::pair<std::wstring, SharedType>>{{UINT_PARAM_NAME,Type::UINT}},
        PRINT_INVOKE_INTERPRETER_FUN
    );

    auto PRINTLN_UINT=std::make_shared<BuiltInFunScope>(
        PRINTLN_NAME,
        Type::UNIT,
        std::vector<std::pair<std::wstring, SharedType>>{{UINT_PARAM_NAME,Type::UINT}},
        PRINTLN_INVOKE_INTERPRETER_FUN
    );

    auto PRINT_LONG=std::make_shared<BuiltInFunScope>(
        PRINT_NAME,
        Type::UNIT,
        std::vector<std::pair<std::wstring, SharedType>>{{LONG_PARAM_NAME,Type::LONG}},
        PRINT_INVOKE_INTERPRETER_FUN
    );

    auto PRINTLN_LONG=std::make_shared<BuiltInFunScope>(
        PRINTLN_NAME,
        Type::UNIT,
        std::vector<std::pair<std::wstring, SharedType>>{{LONG_PARAM_NAME,Type::LONG}},
        PRINTLN_INVOKE_INTERPRETER_FUN
    );

    auto PRINT_ULONG=std::make_shared<BuiltInFunScope>(
        PRINT_NAME,
        Type::UNIT,
        std::vector<std::pair<std::wstring, SharedType>>{{ULONG_PARAM_NAME,Type::ULONG}},
        PRINT_INVOKE_INTERPRETER_FUN
    );

    auto PRINTLN_ULONG=std::make_shared<BuiltInFunScope>(
        PRINTLN_NAME,
        Type::UNIT,
        std::vector<std::pair<std::wstring, SharedType>>{{ULONG_PARAM_NAME,Type::ULONG}},
        PRINTLN_INVOKE_INTERPRETER_FUN
    );

    auto PRINT_FLOAT=std::make_shared<BuiltInFunScope>(
        PRINT_NAME,
        Type::UNIT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME,Type::FLOAT}},
        PRINT_INVOKE_INTERPRETER_FUN
    );

    auto PRINTLN_FLOAT=std::make_shared<BuiltInFunScope>(
        PRINTLN_NAME,
        Type::UNIT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME,Type::FLOAT}},
        PRINTLN_INVOKE_INTERPRETER_FUN
    );

    auto PRINT_DOUBLE=std::make_shared<BuiltInFunScope>(
        PRINT_NAME,
        Type::UNIT,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME,Type::DOUBLE}},
        PRINT_INVOKE_INTERPRETER_FUN
    );

    auto PRINTLN_DOUBLE=std::make_shared<BuiltInFunScope>(
        PRINTLN_NAME,
        Type::UNIT,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME,Type::DOUBLE}},
        PRINTLN_INVOKE_INTERPRETER_FUN
    );

    auto PRINT_CHAR=std::make_shared<BuiltInFunScope>(
        PRINT_NAME,
        Type::UNIT,
        std::vector<std::pair<std::wstring, SharedType>>{{CHAR_PARAM_NAME,Type::CHAR}},
        PRINT_INVOKE_INTERPRETER_FUN
    );

    auto PRINTLN_CHAR=std::make_shared<BuiltInFunScope>(
        PRINTLN_NAME,
        Type::UNIT,
        std::vector<std::pair<std::wstring, SharedType>>{{CHAR_PARAM_NAME,Type::CHAR}},
        PRINTLN_INVOKE_INTERPRETER_FUN
    );

    auto PRINT_STRING=std::make_shared<BuiltInFunScope>(
        PRINT_NAME,
        Type::UNIT,
        std::vector<std::pair<std::wstring, SharedType>>{{STRING_PARAM_NAME,Type::STRING}},
        PRINT_INVOKE_INTERPRETER_FUN
    );

    auto PRINTLN_STRING=std::make_shared<BuiltInFunScope>(
        PRINTLN_NAME,
        Type::UNIT,
        std::vector<std::pair<std::wstring, SharedType>>{{STRING_PARAM_NAME,Type::STRING}},
        PRINTLN_INVOKE_INTERPRETER_FUN
    );

    auto PRINT_BOOL=std::make_shared<BuiltInFunScope>(
        PRINT_NAME,
        Type::UNIT,
        std::vector<std::pair<std::wstring, SharedType>>{{BOOL_PARAM_NAME,Type::BOOL}},
        PRINT_INVOKE_INTERPRETER_FUN
    );

    auto PRINTLN_BOOL=std::make_shared<BuiltInFunScope>(
        PRINTLN_NAME,
        Type::UNIT,
        std::vector<std::pair<std::wstring, SharedType>>{{BOOL_PARAM_NAME,Type::BOOL}},
        PRINTLN_INVOKE_INTERPRETER_FUN
    );

    auto PRINT_UNIT=std::make_shared<BuiltInFunScope>(
        PRINT_NAME,
        Type::UNIT,
        std::vector<std::pair<std::wstring, SharedType>>{{UNIT_PARAM_NAME,Type::UNIT}},
        PRINT_INVOKE_INTERPRETER_FUN
    );

    auto PRINTLN_UNIT=std::make_shared<BuiltInFunScope>(
        PRINTLN_NAME,
        Type::UNIT,
        std::vector<std::pair<std::wstring, SharedType>>{{UNIT_PARAM_NAME,Type::UNIT}},
        PRINTLN_INVOKE_INTERPRETER_FUN
    );

    auto builtInFunctions={
        SYSCALL0,
        SYSCALL1,
        SYSCALL2,
        SYSCALL3,
        SYSCALL4,
        SYSCALL5,
        SYSCALL6,
        BRK,
        WRITE_LONG_TO_ADDRESS,
        READ_LONG_FROM_ADDRESS,
        READ,READ_LINE,
        PRINT_INT,PRINTLN_INT,
        PRINT_UINT,PRINTLN_UINT,
        PRINT_LONG,PRINTLN_LONG,
        PRINT_ULONG,PRINTLN_ULONG,
        PRINT_FLOAT,PRINTLN_FLOAT,
        PRINT_DOUBLE,PRINTLN_DOUBLE,
        PRINT_CHAR,PRINTLN_CHAR,
        PRINT_STRING,PRINTLN_STRING,
        PRINT_BOOL,PRINTLN_BOOL,
        PRINT_UNIT,PRINTLN_UNIT,
    };
    auto privateFunctions=fileScope->getPrivateFunctions();
    for(auto builtInFun:builtInFunctions){
        (*privateFunctions)[builtInFun->getDecl()->toString()]=builtInFun;
    }
}

void BuiltInFunScope::addBuiltInFunctionsToBuiltInClasses() {
    addBuiltInFunctionsToIntClass();
    addBuiltInFunctionsToUIntClass();
    addBuiltInFunctionsToLongClass();
    addBuiltInFunctionsToULongClass();
    addBuiltInFunctionsToFloatClass();
    addBuiltInFunctionsToDoubleClass();
    addBuiltInFunctionsToBoolClass();
    addBuiltInFunctionsToCharClass();
    addBuiltInFunctionsToStringClass();
    addBuiltInFunctionsToUnitClass();
    addBuiltInFunctionsToArrayClass();
}

void BuiltInFunScope::addBuiltInFunctionsToIntClass(){

    auto classScope=std::dynamic_pointer_cast<IntClassScope>(Type::INT->getClassScope());
    
    using PrimitiveType=int;

    auto PLUS_INT=getPlusFun<PrimitiveType, IntValue, IntValue>(
        classScope,
        Type::INT,
        INT_PARAM_NAME,
        Type::INT
    );

    auto PLUS_LONG=getPlusFun<PrimitiveType, LongValue, LongValue>(
        classScope,
        Type::LONG,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto PLUS_FLOAT=getPlusFun<PrimitiveType, FloatValue, FloatValue>(
        classScope,
        Type::FLOAT,
        FLOAT_PARAM_NAME,
        Type::FLOAT
    );

    auto PLUS_DOUBLE=getPlusFun<PrimitiveType, DoubleValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        DOUBLE_PARAM_NAME,
        Type::DOUBLE
    );

    auto MINUS_INT=getMinusFun<PrimitiveType, IntValue, IntValue>(
        classScope,
        Type::INT,
        INT_PARAM_NAME,
        Type::INT
    );

    auto MINUS_LONG=getMinusFun<PrimitiveType, LongValue, LongValue>(
        classScope,
        Type::LONG,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto MINUS_FLOAT=getMinusFun<PrimitiveType, FloatValue, FloatValue>(
        classScope,
        Type::FLOAT,
        FLOAT_PARAM_NAME,
        Type::FLOAT
    );

    auto MINUS_DOUBLE=getMinusFun<PrimitiveType, DoubleValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        DOUBLE_PARAM_NAME,
        Type::DOUBLE
    );

    auto TIMES_INT=getTimesFun<PrimitiveType, IntValue, IntValue>(
        classScope,
        Type::INT,
        INT_PARAM_NAME,
        Type::INT
    );

    auto TIMES_LONG=getTimesFun<PrimitiveType, LongValue, LongValue>(
        classScope,
        Type::LONG,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto TIMES_FLOAT=getTimesFun<PrimitiveType, FloatValue, FloatValue>(
        classScope,
        Type::FLOAT,
        FLOAT_PARAM_NAME,
        Type::FLOAT
    );

    auto TIMES_DOUBLE=getTimesFun<PrimitiveType, DoubleValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        DOUBLE_PARAM_NAME,
        Type::DOUBLE
    );

    auto DIV_INT=getDivFun<PrimitiveType, IntValue, IntValue>(
        classScope,
        Type::INT,
        INT_PARAM_NAME,
        Type::INT
    );

    auto DIV_LONG=getDivFun<PrimitiveType, LongValue, LongValue>(
        classScope,
        Type::LONG,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto DIV_FLOAT=getDivFun<PrimitiveType, FloatValue, FloatValue>(
        classScope,
        Type::FLOAT,
        FLOAT_PARAM_NAME,
        Type::FLOAT
    );

    auto DIV_DOUBLE=getDivFun<PrimitiveType, DoubleValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        DOUBLE_PARAM_NAME,
        Type::DOUBLE
    );

    auto MOD_INT=getModFun<PrimitiveType, IntValue, IntValue>(
        classScope,
        Type::INT,
        INT_PARAM_NAME,
        Type::INT
    );

    auto MOD_LONG=getModFun<PrimitiveType, LongValue, LongValue>(
        classScope,
        Type::LONG,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto COMPARE_TO_INT=getCompareToFun<PrimitiveType, IntValue>(
        classScope,
        INT_PARAM_NAME,
        Type::INT
    );

    auto COMPARE_TO_LONG=getCompareToFun<PrimitiveType, LongValue>(
        classScope,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto COMPARE_TO_FLOAT=getCompareToFun<PrimitiveType, FloatValue>(
        classScope,
        FLOAT_PARAM_NAME,
        Type::FLOAT
    );

    auto COMPARE_TO_DOUBLE=getCompareToFun<PrimitiveType, DoubleValue>(
        classScope,
        DOUBLE_PARAM_NAME,
        Type::DOUBLE
    );

    auto EQUALS=getEqualsFun<PrimitiveType>(
        classScope,
        INT_PARAM_NAME,
        Type::INT
    );

    auto UNARY_PLUS=getUnaryPlusFun<PrimitiveType,IntValue>(classScope,Type::INT);

    auto UNARY_MINUS=getUnaryMinusFun<PrimitiveType,IntValue>(classScope,Type::INT);

    auto INC=getIncFun<PrimitiveType,IntValue>(classScope,Type::INT);

    auto DEC=getDecFun<PrimitiveType,IntValue>(classScope,Type::INT);

    auto TO_INT=getToIntFun<PrimitiveType>(classScope);

    auto TO_UINT=getToUIntFun<PrimitiveType>(classScope);

    auto TO_LONG=getToUIntFun<PrimitiveType>(classScope);

    auto TO_ULONG=getToUIntFun<PrimitiveType>(classScope);

    auto TO_FLOAT=getToFloatFun<PrimitiveType>(classScope);

    auto TO_DOUBLE=getToDoubleFun<PrimitiveType>(classScope);

    auto TO_STRING=getToStringFun<PrimitiveType>(classScope);

    auto TO_CHAR=std::make_shared<BuiltInFunScope>(
        TO_CHAR_NAME,
        Type::CHAR,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){
            auto val=std::dynamic_pointer_cast<IntValue>(interpreter->AX)->getValue();
            wchar_t charValue=static_cast<wchar_t>(val);
            if(isKufrOrUnsupportedCharacter(charValue))
                // TODO: show line number
                throw ContainsKufrOrUnsupportedCharacterException(-1,L"");
            interpreter->AX=std::make_shared<CharValue>(charValue);
        }
    );

    auto SHR=getShrFun<PrimitiveType,IntValue>(classScope,Type::INT);

    auto SHL=getShlFun<PrimitiveType,IntValue>(classScope,Type::INT);

    auto BIT_AND=getBitAndFun<PrimitiveType,IntValue>(
        classScope,
        Type::INT,
        INT_PARAM_NAME
    );

    auto XOR=getXorFun<PrimitiveType,IntValue>(
        classScope,
        Type::INT,
        INT_PARAM_NAME
    );

    auto BIT_OR=getBitOrFun<PrimitiveType,IntValue>(
        classScope,
        Type::INT,
        INT_PARAM_NAME
    );

    auto BIT_NOT=getBitNotFun<PrimitiveType,IntValue>(
        classScope,
        Type::INT
    );

    auto funs={
        PLUS_INT,PLUS_LONG,PLUS_FLOAT,PLUS_DOUBLE,
        MINUS_INT,MINUS_LONG,MINUS_FLOAT,MINUS_DOUBLE,
        TIMES_INT,TIMES_LONG,TIMES_FLOAT,TIMES_DOUBLE,
        DIV_INT,DIV_LONG,DIV_FLOAT,DIV_DOUBLE,
        MOD_INT,MOD_LONG,
        COMPARE_TO_INT,COMPARE_TO_LONG,COMPARE_TO_FLOAT,COMPARE_TO_DOUBLE,
        EQUALS,
        UNARY_PLUS,UNARY_MINUS,
        INC,DEC,
        TO_INT,TO_UINT,TO_LONG,TO_ULONG,
        TO_FLOAT,TO_DOUBLE,TO_STRING,TO_CHAR,
        SHR,SHL,BIT_AND,XOR,BIT_OR,BIT_NOT
    };

    auto publicFuns=classScope->getPublicFunctions();
    for(auto fun:funs){
        (*publicFuns)[fun->getDecl()->toString()]=fun;
    }

}

void BuiltInFunScope::addBuiltInFunctionsToUIntClass(){

    auto classScope=std::dynamic_pointer_cast<UIntClassScope>(Type::UINT->getClassScope());
    
    using PrimitiveType=unsigned int;

    auto PLUS_UINT=getPlusFun<PrimitiveType, UIntValue, UIntValue>(
        classScope,
        Type::UINT,
        UINT_PARAM_NAME,
        Type::UINT
    );

    auto PLUS_ULONG=getPlusFun<PrimitiveType, ULongValue, ULongValue>(
        classScope,
        Type::ULONG,
        ULONG_PARAM_NAME,
        Type::ULONG
    );

    auto MINUS_UINT=getMinusFun<PrimitiveType, UIntValue, UIntValue>(
        classScope,
        Type::UINT,
        UINT_PARAM_NAME,
        Type::UINT
    );

    auto MINUS_ULONG=getMinusFun<PrimitiveType, ULongValue, ULongValue>(
        classScope,
        Type::ULONG,
        ULONG_PARAM_NAME,
        Type::ULONG
    );

    auto TIMES_UINT=getTimesFun<PrimitiveType, UIntValue, UIntValue>(
        classScope,
        Type::UINT,
        UINT_PARAM_NAME,
        Type::UINT
    );

    auto TIMES_ULONG=getTimesFun<PrimitiveType, ULongValue, ULongValue>(
        classScope,
        Type::ULONG,
        ULONG_PARAM_NAME,
        Type::ULONG
    );

    auto DIV_UINT=getDivFun<PrimitiveType, UIntValue, UIntValue>(
        classScope,
        Type::UINT,
        UINT_PARAM_NAME,
        Type::UINT
    );

    auto DIV_ULONG=getDivFun<PrimitiveType, ULongValue, ULongValue>(
        classScope,
        Type::ULONG,
        ULONG_PARAM_NAME,
        Type::ULONG
    );

    auto MOD_UINT=getModFun<PrimitiveType, UIntValue, UIntValue>(
        classScope,
        Type::UINT,
        UINT_PARAM_NAME,
        Type::UINT
    );

    auto MOD_ULONG=getModFun<PrimitiveType, ULongValue, ULongValue>(
        classScope,
        Type::ULONG,
        ULONG_PARAM_NAME,
        Type::ULONG
    );

    auto COMPARE_TO_UINT=getCompareToFun<PrimitiveType, UIntValue>(
        classScope,
        UINT_PARAM_NAME,
        Type::UINT
    );

    auto COMPARE_TO_ULONG=getCompareToFun<PrimitiveType, ULongValue>(
        classScope,
        ULONG_PARAM_NAME,
        Type::ULONG
    );

    auto EQUALS=getEqualsFun<PrimitiveType>(
        classScope,
        UINT_PARAM_NAME,
        Type::UINT
    );

    auto INC=getIncFun<PrimitiveType,UIntValue>(classScope,Type::UINT);

    auto DEC=getDecFun<PrimitiveType,UIntValue>(classScope,Type::UINT);

    auto TO_INT=getToIntFun<PrimitiveType>(classScope);

    auto TO_UINT=getToUIntFun<PrimitiveType>(classScope);

    auto TO_LONG=getToUIntFun<PrimitiveType>(classScope);

    auto TO_ULONG=getToUIntFun<PrimitiveType>(classScope);

    auto TO_FLOAT=getToFloatFun<PrimitiveType>(classScope);

    auto TO_DOUBLE=getToDoubleFun<PrimitiveType>(classScope);

    auto TO_STRING=getToStringFun<PrimitiveType>(classScope);

    auto SHR=getShrFun<PrimitiveType,UIntValue>(classScope,Type::UINT);

    auto SHL=getShlFun<PrimitiveType,UIntValue>(classScope,Type::UINT);

    auto BIT_AND=getBitAndFun<PrimitiveType,UIntValue>(
        classScope,
        Type::UINT,
        UINT_PARAM_NAME
    );

    auto XOR=getXorFun<PrimitiveType,UIntValue>(
        classScope,
        Type::UINT,
        UINT_PARAM_NAME
    );

    auto BIT_OR=getBitOrFun<PrimitiveType,UIntValue>(
        classScope,
        Type::UINT,
        UINT_PARAM_NAME
    );

    auto BIT_NOT=getBitNotFun<PrimitiveType,UIntValue>(
        classScope,
        Type::UINT
    );

    auto funs={
        PLUS_UINT,PLUS_ULONG,
        MINUS_UINT,MINUS_ULONG,
        TIMES_UINT,TIMES_ULONG,
        DIV_UINT,DIV_ULONG,
        MOD_UINT,MOD_ULONG,
        COMPARE_TO_UINT,COMPARE_TO_ULONG,
        EQUALS,
        INC,DEC,
        TO_INT,TO_UINT,TO_LONG,TO_ULONG,
        TO_FLOAT,TO_DOUBLE,TO_STRING,
        SHR,SHL,BIT_AND,XOR,BIT_OR,BIT_NOT
    };

    auto publicFuns=classScope->getPublicFunctions();
    for(auto fun:funs){
        (*publicFuns)[fun->getDecl()->toString()]=fun;
    }

}

void BuiltInFunScope::addBuiltInFunctionsToLongClass(){

    auto classScope=std::dynamic_pointer_cast<LongClassScope>(Type::LONG->getClassScope());
    
    using PrimitiveType=long long;

    auto PLUS_INT=getPlusFun<PrimitiveType, IntValue, LongValue>(
        classScope,
        Type::LONG,
        INT_PARAM_NAME,
        Type::INT
    );

    auto PLUS_LONG=getPlusFun<PrimitiveType, LongValue, LongValue>(
        classScope,
        Type::LONG,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto PLUS_FLOAT=getPlusFun<PrimitiveType, FloatValue, FloatValue>(
        classScope,
        Type::FLOAT,
        FLOAT_PARAM_NAME,
        Type::FLOAT
    );

    auto PLUS_DOUBLE=getPlusFun<PrimitiveType, DoubleValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        DOUBLE_PARAM_NAME,
        Type::DOUBLE
    );

    auto MINUS_INT=getMinusFun<PrimitiveType, IntValue, LongValue>(
        classScope,
        Type::LONG,
        INT_PARAM_NAME,
        Type::INT
    );

    auto MINUS_LONG=getMinusFun<PrimitiveType, LongValue, LongValue>(
        classScope,
        Type::LONG,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto MINUS_FLOAT=getMinusFun<PrimitiveType, FloatValue, FloatValue>(
        classScope,
        Type::FLOAT,
        FLOAT_PARAM_NAME,
        Type::FLOAT
    );

    auto MINUS_DOUBLE=getMinusFun<PrimitiveType, DoubleValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        DOUBLE_PARAM_NAME,
        Type::DOUBLE
    );

    auto TIMES_INT=getTimesFun<PrimitiveType, IntValue, LongValue>(
        classScope,
        Type::LONG,
        INT_PARAM_NAME,
        Type::INT
    );

    auto TIMES_LONG=getTimesFun<PrimitiveType, LongValue, LongValue>(
        classScope,
        Type::LONG,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto TIMES_FLOAT=getTimesFun<PrimitiveType, FloatValue, FloatValue>(
        classScope,
        Type::FLOAT,
        FLOAT_PARAM_NAME,
        Type::FLOAT
    );

    auto TIMES_DOUBLE=getTimesFun<PrimitiveType, DoubleValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        DOUBLE_PARAM_NAME,
        Type::DOUBLE
    );

    auto DIV_INT=getDivFun<PrimitiveType, IntValue, LongValue>(
        classScope,
        Type::LONG,
        INT_PARAM_NAME,
        Type::INT
    );

    auto DIV_LONG=getDivFun<PrimitiveType, LongValue, LongValue>(
        classScope,
        Type::LONG,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto DIV_FLOAT=getDivFun<PrimitiveType, FloatValue, FloatValue>(
        classScope,
        Type::FLOAT,
        FLOAT_PARAM_NAME,
        Type::FLOAT
    );

    auto DIV_DOUBLE=getDivFun<PrimitiveType, DoubleValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        DOUBLE_PARAM_NAME,
        Type::DOUBLE
    );

    auto MOD_INT=getModFun<PrimitiveType, IntValue, LongValue>(
        classScope,
        Type::LONG,
        INT_PARAM_NAME,
        Type::INT
    );

    auto MOD_LONG=getModFun<PrimitiveType, LongValue, LongValue>(
        classScope,
        Type::LONG,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto COMPARE_TO_INT=getCompareToFun<PrimitiveType, IntValue>(
        classScope,
        INT_PARAM_NAME,
        Type::INT
    );

    auto COMPARE_TO_LONG=getCompareToFun<PrimitiveType, LongValue>(
        classScope,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto COMPARE_TO_FLOAT=getCompareToFun<PrimitiveType, FloatValue>(
        classScope,
        FLOAT_PARAM_NAME,
        Type::FLOAT
    );

    auto COMPARE_TO_DOUBLE=getCompareToFun<PrimitiveType, DoubleValue>(
        classScope,
        DOUBLE_PARAM_NAME,
        Type::DOUBLE
    );

    auto EQUALS=getEqualsFun<PrimitiveType>(
        classScope,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto UNARY_PLUS=getUnaryPlusFun<PrimitiveType,LongValue>(classScope,Type::LONG);

    auto UNARY_MINUS=getUnaryMinusFun<PrimitiveType,LongValue>(classScope,Type::LONG);

    auto INC=getIncFun<PrimitiveType,LongValue>(classScope,Type::LONG);

    auto DEC=getDecFun<PrimitiveType,LongValue>(classScope,Type::LONG);

    auto TO_INT=getToIntFun<PrimitiveType>(classScope);

    auto TO_UINT=getToUIntFun<PrimitiveType>(classScope);

    auto TO_LONG=getToUIntFun<PrimitiveType>(classScope);

    auto TO_ULONG=getToUIntFun<PrimitiveType>(classScope);

    auto TO_FLOAT=getToFloatFun<PrimitiveType>(classScope);

    auto TO_DOUBLE=getToDoubleFun<PrimitiveType>(classScope);

    auto TO_STRING=getToStringFun<PrimitiveType>(classScope);

    auto SHR=getShrFun<PrimitiveType,LongValue>(classScope,Type::LONG);

    auto SHL=getShlFun<PrimitiveType,LongValue>(classScope,Type::LONG);

    auto BIT_AND=getBitAndFun<PrimitiveType,LongValue>(
        classScope,
        Type::LONG,
        LONG_PARAM_NAME
    );

    auto XOR=getXorFun<PrimitiveType,LongValue>(
        classScope,
        Type::LONG,
        LONG_PARAM_NAME
    );

    auto BIT_OR=getBitOrFun<PrimitiveType,LongValue>(
        classScope,
        Type::LONG,
        LONG_PARAM_NAME
    );

    auto BIT_NOT=getBitNotFun<PrimitiveType,LongValue>(
        classScope,
        Type::LONG
    );

    auto funs={
        PLUS_INT,PLUS_LONG,PLUS_FLOAT,PLUS_DOUBLE,
        MINUS_INT,MINUS_LONG,MINUS_FLOAT,MINUS_DOUBLE,
        TIMES_INT,TIMES_LONG,TIMES_FLOAT,TIMES_DOUBLE,
        DIV_INT,DIV_LONG,DIV_FLOAT,DIV_DOUBLE,
        MOD_INT,MOD_LONG,
        COMPARE_TO_INT,COMPARE_TO_LONG,COMPARE_TO_FLOAT,COMPARE_TO_DOUBLE,
        EQUALS,
        UNARY_PLUS,UNARY_MINUS,
        INC,DEC,
        TO_INT,TO_UINT,TO_LONG,TO_ULONG,
        TO_FLOAT,TO_DOUBLE,TO_STRING,
        SHR,SHL,BIT_AND,XOR,BIT_OR,BIT_NOT
    };

    auto publicFuns=classScope->getPublicFunctions();
    for(auto fun:funs){
        (*publicFuns)[fun->getDecl()->toString()]=fun;
    }

}

void BuiltInFunScope::addBuiltInFunctionsToULongClass(){

    auto classScope=std::dynamic_pointer_cast<ULongClassScope>(Type::ULONG->getClassScope());
    
    using PrimitiveType=unsigned long long;

    auto PLUS_UINT=getPlusFun<PrimitiveType, UIntValue, ULongValue>(
        classScope,
        Type::ULONG,
        UINT_PARAM_NAME,
        Type::UINT
    );

    auto PLUS_ULONG=getPlusFun<PrimitiveType, ULongValue, ULongValue>(
        classScope,
        Type::ULONG,
        ULONG_PARAM_NAME,
        Type::ULONG
    );

    auto MINUS_UINT=getMinusFun<PrimitiveType, UIntValue, ULongValue>(
        classScope,
        Type::ULONG,
        UINT_PARAM_NAME,
        Type::UINT
    );

    auto MINUS_ULONG=getMinusFun<PrimitiveType, ULongValue, ULongValue>(
        classScope,
        Type::ULONG,
        ULONG_PARAM_NAME,
        Type::ULONG
    );

    auto TIMES_UINT=getTimesFun<PrimitiveType, UIntValue, ULongValue>(
        classScope,
        Type::ULONG,
        UINT_PARAM_NAME,
        Type::UINT
    );

    auto TIMES_ULONG=getTimesFun<PrimitiveType, ULongValue, ULongValue>(
        classScope,
        Type::ULONG,
        ULONG_PARAM_NAME,
        Type::ULONG
    );

    auto DIV_UINT=getDivFun<PrimitiveType, UIntValue, ULongValue>(
        classScope,
        Type::ULONG,
        UINT_PARAM_NAME,
        Type::UINT
    );

    auto DIV_ULONG=getDivFun<PrimitiveType, ULongValue, ULongValue>(
        classScope,
        Type::ULONG,
        ULONG_PARAM_NAME,
        Type::ULONG
    );

    auto MOD_UINT=getModFun<PrimitiveType, UIntValue, ULongValue>(
        classScope,
        Type::ULONG,
        UINT_PARAM_NAME,
        Type::UINT
    );

    auto MOD_ULONG=getModFun<PrimitiveType, ULongValue, ULongValue>(
        classScope,
        Type::ULONG,
        ULONG_PARAM_NAME,
        Type::ULONG
    );

    auto COMPARE_TO_UINT=getCompareToFun<PrimitiveType, UIntValue>(
        classScope,
        UINT_PARAM_NAME,
        Type::UINT
    );

    auto COMPARE_TO_ULONG=getCompareToFun<PrimitiveType, ULongValue>(
        classScope,
        ULONG_PARAM_NAME,
        Type::ULONG
    );

    auto EQUALS=getEqualsFun<PrimitiveType>(
        classScope,
        ULONG_PARAM_NAME,
        Type::ULONG
    );

    auto INC=getIncFun<PrimitiveType,ULongValue>(classScope,Type::ULONG);

    auto DEC=getDecFun<PrimitiveType,ULongValue>(classScope,Type::ULONG);

    auto TO_INT=getToIntFun<PrimitiveType>(classScope);

    auto TO_UINT=getToUIntFun<PrimitiveType>(classScope);

    auto TO_LONG=getToUIntFun<PrimitiveType>(classScope);

    auto TO_ULONG=getToUIntFun<PrimitiveType>(classScope);

    auto TO_FLOAT=getToFloatFun<PrimitiveType>(classScope);

    auto TO_DOUBLE=getToDoubleFun<PrimitiveType>(classScope);

    auto TO_STRING=getToStringFun<PrimitiveType>(classScope);

    auto SHR=getShrFun<PrimitiveType,ULongValue>(classScope,Type::ULONG);

    auto SHL=getShlFun<PrimitiveType,ULongValue>(classScope,Type::ULONG);

    auto BIT_AND=getBitAndFun<PrimitiveType,ULongValue>(
        classScope,
        Type::ULONG,
        ULONG_PARAM_NAME
    );

    auto XOR=getXorFun<PrimitiveType,ULongValue>(
        classScope,
        Type::ULONG,
        ULONG_PARAM_NAME
    );

    auto BIT_OR=getBitOrFun<PrimitiveType,ULongValue>(
        classScope,
        Type::ULONG,
        ULONG_PARAM_NAME
    );

    auto BIT_NOT=getBitNotFun<PrimitiveType,ULongValue>(
        classScope,
        Type::ULONG
    );

    auto funs={
        PLUS_UINT,PLUS_ULONG,
        MINUS_UINT,MINUS_ULONG,
        TIMES_UINT,TIMES_ULONG,
        DIV_UINT,DIV_ULONG,
        MOD_UINT,MOD_ULONG,
        COMPARE_TO_UINT,COMPARE_TO_ULONG,
        EQUALS,
        INC,DEC,
        TO_INT,TO_UINT,TO_LONG,TO_ULONG,
        TO_FLOAT,TO_DOUBLE,TO_STRING,
        SHR,SHL,BIT_AND,XOR,BIT_OR,BIT_NOT
    };

    auto publicFuns=classScope->getPublicFunctions();
    for(auto fun:funs){
        (*publicFuns)[fun->getDecl()->toString()]=fun;
    }

}

void BuiltInFunScope::addBuiltInFunctionsToFloatClass(){

    auto classScope=std::dynamic_pointer_cast<FloatClassScope>(Type::FLOAT->getClassScope());
    
    using PrimitiveType=float;

    auto PLUS_INT=getPlusFun<PrimitiveType, IntValue, FloatValue>(
        classScope,
        Type::FLOAT,
        INT_PARAM_NAME,
        Type::INT
    );

    auto PLUS_LONG=getPlusFun<PrimitiveType, LongValue, FloatValue>(
        classScope,
        Type::FLOAT,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto PLUS_FLOAT=getPlusFun<PrimitiveType, FloatValue, FloatValue>(
        classScope,
        Type::FLOAT,
        FLOAT_PARAM_NAME,
        Type::FLOAT
    );

    auto PLUS_DOUBLE=getPlusFun<PrimitiveType, DoubleValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        DOUBLE_PARAM_NAME,
        Type::DOUBLE
    );

    auto MINUS_INT=getMinusFun<PrimitiveType, IntValue, FloatValue>(
        classScope,
        Type::FLOAT,
        INT_PARAM_NAME,
        Type::INT
    );

    auto MINUS_LONG=getMinusFun<PrimitiveType, LongValue, FloatValue>(
        classScope,
        Type::FLOAT,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto MINUS_FLOAT=getMinusFun<PrimitiveType, FloatValue, FloatValue>(
        classScope,
        Type::FLOAT,
        FLOAT_PARAM_NAME,
        Type::FLOAT
    );

    auto MINUS_DOUBLE=getMinusFun<PrimitiveType, DoubleValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        DOUBLE_PARAM_NAME,
        Type::DOUBLE
    );

    auto TIMES_INT=getTimesFun<PrimitiveType, IntValue, FloatValue>(
        classScope,
        Type::FLOAT,
        INT_PARAM_NAME,
        Type::INT
    );

    auto TIMES_LONG=getTimesFun<PrimitiveType, LongValue, FloatValue>(
        classScope,
        Type::FLOAT,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto TIMES_FLOAT=getTimesFun<PrimitiveType, FloatValue, FloatValue>(
        classScope,
        Type::FLOAT,
        FLOAT_PARAM_NAME,
        Type::FLOAT
    );

    auto TIMES_DOUBLE=getTimesFun<PrimitiveType, DoubleValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        DOUBLE_PARAM_NAME,
        Type::DOUBLE
    );

    auto DIV_INT=getDivFun<PrimitiveType, IntValue, FloatValue>(
        classScope,
        Type::FLOAT,
        INT_PARAM_NAME,
        Type::INT
    );

    auto DIV_LONG=getDivFun<PrimitiveType, LongValue, FloatValue>(
        classScope,
        Type::FLOAT,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto DIV_FLOAT=getDivFun<PrimitiveType, FloatValue, FloatValue>(
        classScope,
        Type::FLOAT,
        FLOAT_PARAM_NAME,
        Type::FLOAT
    );

    auto DIV_DOUBLE=getDivFun<PrimitiveType, DoubleValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        DOUBLE_PARAM_NAME,
        Type::DOUBLE
    );

    auto COMPARE_TO_INT=getCompareToFun<PrimitiveType, IntValue>(
        classScope,
        INT_PARAM_NAME,
        Type::INT
    );

    auto COMPARE_TO_LONG=getCompareToFun<PrimitiveType, LongValue>(
        classScope,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto COMPARE_TO_FLOAT=getCompareToFun<PrimitiveType, FloatValue>(
        classScope,
        FLOAT_PARAM_NAME,
        Type::FLOAT
    );

    auto COMPARE_TO_DOUBLE=getCompareToFun<PrimitiveType, DoubleValue>(
        classScope,
        DOUBLE_PARAM_NAME,
        Type::DOUBLE
    );

    auto EQUALS=getEqualsFun<PrimitiveType>(
        classScope,
        FLOAT_PARAM_NAME,
        Type::FLOAT
    );

    auto UNARY_PLUS=getUnaryPlusFun<PrimitiveType,FloatValue>(classScope,Type::FLOAT);

    auto UNARY_MINUS=getUnaryMinusFun<PrimitiveType,FloatValue>(classScope,Type::FLOAT);

    auto INC=getIncFun<PrimitiveType,FloatValue>(classScope,Type::FLOAT);

    auto DEC=getDecFun<PrimitiveType,FloatValue>(classScope,Type::FLOAT);

    auto TO_INT=getToIntFun<PrimitiveType>(classScope);

    auto TO_UINT=getToUIntFun<PrimitiveType>(classScope);

    auto TO_LONG=getToUIntFun<PrimitiveType>(classScope);

    auto TO_ULONG=getToUIntFun<PrimitiveType>(classScope);

    auto TO_FLOAT=getToFloatFun<PrimitiveType>(classScope);

    auto TO_DOUBLE=getToDoubleFun<PrimitiveType>(classScope);

    auto TO_STRING=getToStringFun<PrimitiveType>(classScope);

    auto funs={
        PLUS_INT,PLUS_LONG,PLUS_FLOAT,PLUS_DOUBLE,
        MINUS_INT,MINUS_LONG,MINUS_FLOAT,MINUS_DOUBLE,
        TIMES_INT,TIMES_LONG,TIMES_FLOAT,TIMES_DOUBLE,
        DIV_INT,DIV_LONG,DIV_FLOAT,DIV_DOUBLE,
        COMPARE_TO_INT,COMPARE_TO_LONG,COMPARE_TO_FLOAT,COMPARE_TO_DOUBLE,
        EQUALS,
        UNARY_PLUS,UNARY_MINUS,
        INC,DEC,
        TO_INT,TO_UINT,TO_LONG,TO_ULONG,
        TO_FLOAT,TO_DOUBLE,TO_STRING
    };

    auto publicFuns=classScope->getPublicFunctions();
    for(auto fun:funs){
        (*publicFuns)[fun->getDecl()->toString()]=fun;
    }

}

void BuiltInFunScope::addBuiltInFunctionsToDoubleClass(){

    auto classScope=std::dynamic_pointer_cast<DoubleClassScope>(Type::DOUBLE->getClassScope());
    
    using PrimitiveType=long double;

    auto PLUS_INT=getPlusFun<PrimitiveType, IntValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        INT_PARAM_NAME,
        Type::INT
    );

    auto PLUS_LONG=getPlusFun<PrimitiveType, LongValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto PLUS_FLOAT=getPlusFun<PrimitiveType, FloatValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        FLOAT_PARAM_NAME,
        Type::FLOAT
    );

    auto PLUS_DOUBLE=getPlusFun<PrimitiveType, DoubleValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        DOUBLE_PARAM_NAME,
        Type::DOUBLE
    );

    auto MINUS_INT=getMinusFun<PrimitiveType, IntValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        INT_PARAM_NAME,
        Type::INT
    );

    auto MINUS_LONG=getMinusFun<PrimitiveType, LongValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto MINUS_FLOAT=getMinusFun<PrimitiveType, FloatValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        FLOAT_PARAM_NAME,
        Type::FLOAT
    );

    auto MINUS_DOUBLE=getMinusFun<PrimitiveType, DoubleValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        DOUBLE_PARAM_NAME,
        Type::DOUBLE
    );

    auto TIMES_INT=getTimesFun<PrimitiveType, IntValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        INT_PARAM_NAME,
        Type::INT
    );

    auto TIMES_LONG=getTimesFun<PrimitiveType, LongValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto TIMES_FLOAT=getTimesFun<PrimitiveType, FloatValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        FLOAT_PARAM_NAME,
        Type::FLOAT
    );

    auto TIMES_DOUBLE=getTimesFun<PrimitiveType, DoubleValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        DOUBLE_PARAM_NAME,
        Type::DOUBLE
    );

    auto DIV_INT=getDivFun<PrimitiveType, IntValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        INT_PARAM_NAME,
        Type::INT
    );

    auto DIV_LONG=getDivFun<PrimitiveType, LongValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto DIV_FLOAT=getDivFun<PrimitiveType, FloatValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        FLOAT_PARAM_NAME,
        Type::FLOAT
    );

    auto DIV_DOUBLE=getDivFun<PrimitiveType, DoubleValue, DoubleValue>(
        classScope,
        Type::DOUBLE,
        DOUBLE_PARAM_NAME,
        Type::DOUBLE
    );

    auto COMPARE_TO_INT=getCompareToFun<PrimitiveType, IntValue>(
        classScope,
        INT_PARAM_NAME,
        Type::INT
    );

    auto COMPARE_TO_LONG=getCompareToFun<PrimitiveType, LongValue>(
        classScope,
        LONG_PARAM_NAME,
        Type::LONG
    );

    auto COMPARE_TO_FLOAT=getCompareToFun<PrimitiveType, FloatValue>(
        classScope,
        FLOAT_PARAM_NAME,
        Type::FLOAT
    );

    auto COMPARE_TO_DOUBLE=getCompareToFun<PrimitiveType, DoubleValue>(
        classScope,
        DOUBLE_PARAM_NAME,
        Type::DOUBLE
    );

    auto EQUALS=getEqualsFun<PrimitiveType>(
        classScope,
        DOUBLE_PARAM_NAME,
        Type::DOUBLE
    );

    auto UNARY_PLUS=getUnaryPlusFun<PrimitiveType,DoubleValue>(classScope,Type::DOUBLE);

    auto UNARY_MINUS=getUnaryMinusFun<PrimitiveType,DoubleValue>(classScope,Type::DOUBLE);

    auto INC=getIncFun<PrimitiveType,DoubleValue>(classScope,Type::DOUBLE);

    auto DEC=getDecFun<PrimitiveType,DoubleValue>(classScope,Type::DOUBLE);

    auto TO_INT=getToIntFun<PrimitiveType>(classScope);

    auto TO_UINT=getToUIntFun<PrimitiveType>(classScope);

    auto TO_LONG=getToUIntFun<PrimitiveType>(classScope);

    auto TO_ULONG=getToUIntFun<PrimitiveType>(classScope);

    auto TO_FLOAT=getToFloatFun<PrimitiveType>(classScope);

    auto TO_DOUBLE=getToDoubleFun<PrimitiveType>(classScope);

    auto TO_STRING=getToStringFun<PrimitiveType>(classScope);

    auto funs={
        PLUS_INT,PLUS_LONG,PLUS_FLOAT,PLUS_DOUBLE,
        MINUS_INT,MINUS_LONG,MINUS_FLOAT,MINUS_DOUBLE,
        TIMES_INT,TIMES_LONG,TIMES_FLOAT,TIMES_DOUBLE,
        DIV_INT,DIV_LONG,DIV_FLOAT,DIV_DOUBLE,
        COMPARE_TO_INT,COMPARE_TO_LONG,COMPARE_TO_FLOAT,COMPARE_TO_DOUBLE,
        EQUALS,
        UNARY_PLUS,UNARY_MINUS,
        INC,DEC,
        TO_INT,TO_UINT,TO_LONG,TO_ULONG,
        TO_FLOAT,TO_DOUBLE,TO_STRING
    };

    auto publicFuns=classScope->getPublicFunctions();
    for(auto fun:funs){
        (*publicFuns)[fun->getDecl()->toString()]=fun;
    }

}

void BuiltInFunScope::addBuiltInFunctionsToBoolClass(){

    auto classScope=std::dynamic_pointer_cast<BoolClassScope>(Type::BOOL->getClassScope());
    
    using PrimitiveType=bool;

    auto NOT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::LOGICAL_NOT_NAME,
        Type::BOOL,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){
            auto val=std::dynamic_pointer_cast<BoolValue>(interpreter->AX)->getValue();
            interpreter->AX=std::make_shared<BoolValue>(!val);
        },
        true
    );

    auto EQUALS=getEqualsFun<PrimitiveType>(
        classScope,
        BOOL_PARAM_NAME,
        Type::BOOL
    );

    auto TO_STRING=std::make_shared<BuiltInFunScope>(
        TO_STRING_NAME,
        Type::STRING,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){
            interpreter->AX=std::make_shared<StringValue>(interpreter->AX->toString());
        }
    );

    auto BIT_AND=getBitAndFun<PrimitiveType,BoolValue>(
        classScope,
        Type::BOOL,
        BOOL_PARAM_NAME
    );

    auto XOR=getXorFun<PrimitiveType,BoolValue>(
        classScope,
        Type::BOOL,
        BOOL_PARAM_NAME
    );

    auto BIT_OR=getBitOrFun<PrimitiveType,BoolValue>(
        classScope,
        Type::BOOL,
        BOOL_PARAM_NAME
    );

    auto funs={
        NOT,
        EQUALS,
        TO_STRING,
        BIT_AND,XOR,BIT_OR,
    };

    auto publicFuns=classScope->getPublicFunctions();
    for(auto fun:funs){
        (*publicFuns)[fun->getDecl()->toString()]=fun;
    }
    
}

void BuiltInFunScope::addBuiltInFunctionsToCharClass() {
    
    auto classScope=std::dynamic_pointer_cast<CharClassScope>(Type::CHAR->getClassScope());
    
    using PrimitiveType=wchar_t;

    auto PLUS_INT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::PLUS_NAME,
        Type::CHAR,
        std::vector<std::pair<std::wstring, SharedType>>{{INT_PARAM_NAME,Type::INT}},
        [](Interpreter* interpreter){
            auto a=std::dynamic_pointer_cast<CharValue>(interpreter->AX)->getValue();
            auto b=std::dynamic_pointer_cast<IntValue>(interpreter->CX)->getValue();
            auto charValue=static_cast<wchar_t>(a+b);
            if(isKufrOrUnsupportedCharacter(charValue))
                // TODO: show line number
                throw ContainsKufrOrUnsupportedCharacterException(-1,L"");
            interpreter->AX=std::make_shared<CharValue>(charValue);
        },
        true
    );

    auto PLUS_STRING=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::PLUS_NAME,
        Type::STRING,
        std::vector<std::pair<std::wstring, SharedType>>{{STRING_PARAM_NAME,Type::STRING}},
        [](Interpreter* interpreter){
            auto a=interpreter->AX->toString();
            auto b=interpreter->CX->toString();
            interpreter->AX=std::make_shared<StringValue>(a+b);
        },
        true
    );

    auto MINUS_INT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::MINUS_NAME,
        Type::CHAR,
        std::vector<std::pair<std::wstring, SharedType>>{{INT_PARAM_NAME,Type::INT}},
        [](Interpreter* interpreter){
            auto a=std::dynamic_pointer_cast<CharValue>(interpreter->AX)->getValue();
            auto b=std::dynamic_pointer_cast<IntValue>(interpreter->CX)->getValue();
            auto charValue=static_cast<wchar_t>(a-b);
            if(isKufrOrUnsupportedCharacter(charValue))
                // TODO: show line number
                throw ContainsKufrOrUnsupportedCharacterException(-1,L"");
            interpreter->AX=std::make_shared<CharValue>(charValue);
        },
        true
    );

    auto MINUS_CHAR=getMinusFun<PrimitiveType, CharValue, IntValue>(
        classScope,
        Type::INT,
        CHAR_PARAM_NAME,
        Type::CHAR
    );

    auto COMPARE_TO_CHAR=getCompareToFun<PrimitiveType, CharValue>(
        classScope,
        CHAR_PARAM_NAME,
        Type::CHAR
    );

    auto EQUALS=getEqualsFun<PrimitiveType>(
        classScope,
        CHAR_PARAM_NAME,
        Type::CHAR
    );

    auto TO_CHAR=std::make_shared<BuiltInFunScope>(
        TO_CHAR_NAME,
        Type::CHAR,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){
            // Nothing
        }
    );

    auto TO_STRING=std::make_shared<BuiltInFunScope>(
        TO_STRING_NAME,
        Type::STRING,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){
            auto val=interpreter->AX->toString();
            interpreter->AX=std::make_shared<StringValue>(val);
        }
    );

    auto funs={
        PLUS_INT,PLUS_STRING,
        MINUS_INT,MINUS_CHAR,
        COMPARE_TO_CHAR,
        EQUALS,
        TO_CHAR,TO_STRING
    };

    auto publicFuns=classScope->getPublicFunctions();
    for(auto fun:funs){
        (*publicFuns)[fun->getDecl()->toString()]=fun;
    }

}

void BuiltInFunScope::addBuiltInFunctionsToStringClass() {
        
    auto classScope=std::dynamic_pointer_cast<StringClassScope>(Type::STRING->getClassScope());

    auto PLUS_STRING=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::PLUS_NAME,
        Type::STRING,
        std::vector<std::pair<std::wstring, SharedType>>{{STRING_PARAM_NAME,Type::STRING}},
        [](Interpreter* interpreter){
            auto a=interpreter->AX->toString();
            auto b=interpreter->CX->toString();
            interpreter->AX=std::make_shared<StringValue>(a+b);
        },
        true
    );

    auto PLUS_CHAR=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::PLUS_NAME,
        Type::STRING,
        std::vector<std::pair<std::wstring, SharedType>>{{CHAR_PARAM_NAME,Type::CHAR}},
        [](Interpreter* interpreter){
            auto a=interpreter->AX->toString();
            auto b=interpreter->CX->toString();
            interpreter->AX=std::make_shared<StringValue>(a+b);
        },
        true
    );

    auto EQUALS=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::EQUALS_NAME,
        Type::BOOL,
        std::vector<std::pair<std::wstring, SharedType>>{{STRING_PARAM_NAME,Type::STRING}},
        [](Interpreter* interpreter){
            auto a=interpreter->AX->toString();
            auto b=interpreter->CX->toString();
            interpreter->AX=std::make_shared<BoolValue>(a==b);
        },
        true
    );

    auto TO_INT=std::make_shared<BuiltInFunScope>(
        TO_INT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){
            auto val=interpreter->AX->toString();
            try{
                auto value=std::stoi(val);
                interpreter->AX=std::make_shared<IntValue>(value);
            }catch(std::exception e){
                throw NumberFormatException(val);
            }
        }
    );

    auto TO_UINT=std::make_shared<BuiltInFunScope>(
        TO_UINT_NAME,
        Type::UINT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){
            auto val=interpreter->AX->toString();
            try{
                auto value=std::stoull(val);

                if(value>std::numeric_limits<unsigned int>().max())
                    throw NumberFormatException(val);

                interpreter->AX=std::make_shared<UIntValue>(value);
            }catch(std::exception e){
                throw NumberFormatException(val);
            }
        }
    );

    auto TO_LONG=std::make_shared<BuiltInFunScope>(
        TO_LONG_NAME,
        Type::LONG,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){
            auto val=interpreter->AX->toString();
            try{
                auto value=std::stoll(val);
                interpreter->AX=std::make_shared<LongValue>(value);
            }catch(std::exception e){
                throw NumberFormatException(val);
            }
        }
    );

    auto TO_ULONG=std::make_shared<BuiltInFunScope>(
        TO_ULONG_NAME,
        Type::ULONG,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){
            auto val=interpreter->AX->toString();
            try{
                auto value=std::stoull(val);
                interpreter->AX=std::make_shared<ULongValue>(value);
            }catch(std::exception e){
                throw NumberFormatException(val);
            }
        }
    );

    auto TO_FLOAT=std::make_shared<BuiltInFunScope>(
        TO_FLOAT_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){
            auto val=interpreter->AX->toString();
            try{
                auto value=std::stof(val);
                interpreter->AX=std::make_shared<FloatValue>(value);
            }catch(std::exception e){
                throw NumberFormatException(val);
            }
        }
    );

    auto TO_DOUBLE=std::make_shared<BuiltInFunScope>(
        TO_DOUBLE_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){
            auto val=interpreter->AX->toString();
            try{
                auto value=std::stold(val);
                interpreter->AX=std::make_shared<DoubleValue>(value);
            }catch(std::exception e){
                throw NumberFormatException(val);
            }
        }
    );

    auto TO_STRING=std::make_shared<BuiltInFunScope>(
        TO_STRING_NAME,
        Type::STRING,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){
            auto val=interpreter->AX;
            interpreter->AX=std::make_shared<StringValue>(val->toString());
        }
    );

    auto funs={
        PLUS_STRING,PLUS_CHAR,
        EQUALS,
        TO_INT,TO_UINT,TO_LONG,TO_ULONG,
        TO_FLOAT,TO_DOUBLE,
        TO_STRING
    };

    auto publicFuns=classScope->getPublicFunctions();
    for(auto fun:funs){
        (*publicFuns)[fun->getDecl()->toString()]=fun;
    }

}

void BuiltInFunScope::addBuiltInFunctionsToUnitClass(){

    auto TO_STRING=std::make_shared<BuiltInFunScope>(
        TO_STRING_NAME,
        Type::STRING,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){
            auto val=interpreter->AX;
            interpreter->AX=std::make_shared<StringValue>(val->toString());
        }
    );

    auto publicFuns=Type::UNIT->getClassScope()->getPublicFunctions();
   
    (*publicFuns)[TO_STRING->getDecl()->toString()]=TO_STRING;

}

void BuiltInFunScope::addBuiltInFunctionsToArrayClass(){
    auto classScope=Type::ARRAY_CLASS;

    auto genericType=std::make_shared<Type>(std::make_shared<std::wstring>(L""));

    auto GET=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::GET_NAME,
        genericType,
        std::vector<std::pair<std::wstring, SharedType>>{{INDEX_PARAM_NAME,Type::INT}},
        [](Interpreter* interpreter){
            auto arrayAddress=std::dynamic_pointer_cast<RefValue>(interpreter->AX)->getAddress();
            auto index=std::dynamic_pointer_cast<IntValue>(interpreter->CX)->getValue();
            auto arraySize=std::dynamic_pointer_cast<IntValue>(interpreter->memory[arrayAddress])->getValue();
            if(index>=arraySize)
                throw ArrayIndexOutOfRangeException(arraySize,index);
            interpreter->AX=interpreter->memory[arrayAddress+index+1];
        },
        true
    );

    auto SET=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::SET_NAME,
        Type::UNIT,
        std::vector<std::pair<std::wstring, SharedType>>{
            {INDEX_PARAM_NAME,Type::INT},
            {VALUE_PARAM_NAME,genericType},
        },
        [](Interpreter* interpreter){
            auto arrayAddress=std::dynamic_pointer_cast<RefValue>(interpreter->AX)->getAddress();
            auto index=std::dynamic_pointer_cast<IntValue>(interpreter->CX)->getValue();
            auto arraySize=std::dynamic_pointer_cast<IntValue>(interpreter->memory[arrayAddress])->getValue();
            if(index>=arraySize)
                throw ArrayIndexOutOfRangeException(arraySize,index);
            auto value=interpreter->DX;
            interpreter->memory[arrayAddress+index+1]=value;
            interpreter->AX=std::make_shared<UnitValue>();
        },
        true
    );

    auto IS_NOT_EMPTY=std::make_shared<BuiltInFunScope>(
        IS_NOT_EMPTY_NAME,
        Type::BOOL,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){
            auto arrayAddress=std::dynamic_pointer_cast<RefValue>(interpreter->AX)->getAddress();
            auto size=std::dynamic_pointer_cast<IntValue>(interpreter->memory[arrayAddress])->getValue();
            interpreter->AX=std::make_shared<BoolValue>(size!=0);
        }
    );

    ArrayClassScope::GET=GET;
    ArrayClassScope::SET=SET;

    auto funs={
        GET,
        SET,
        IS_NOT_EMPTY,
    };

    auto publicFuns=classScope->getPublicFunctions();
    for(auto fun:funs){
        (*publicFuns)[fun->getDecl()->toString()]=fun;
    }
}