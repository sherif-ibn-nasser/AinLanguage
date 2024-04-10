#include "BuiltInFunScope.hpp"
#include "ArrayIndexOutOfRangeException.hpp"
#include "Assembler.hpp"
#include "BoolClassScope.hpp"
#include "BoolValue.hpp"
#include "ByteValue.hpp"
#include "CharClassScope.hpp"
#include "CharValue.hpp"
#include "Compiler.hpp"
#include "ContainsKufrOrUnsupportedCharacterException.hpp"
#include "DoubleClassScope.hpp"
#include "DoubleValue.hpp"
#include "FloatClassScope.hpp"
#include "FloatValue.hpp"
#include "FunParam.hpp"
#include "IntClassScope.hpp"
#include "IntValue.hpp"
#include "Interpreter.hpp"
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
#include "ByteClassScope.hpp"
#include "UByteClassScope.hpp"
#include "UByteValue.hpp"
#include "UIntClassScope.hpp"
#include "UIntValue.hpp"
#include "ULongClassScope.hpp"
#include "ULongValue.hpp"
#include "VoidValue.hpp"
#include "ArrayClassScope.hpp"
#include "Variable.hpp"
#include "ainio.hpp"
#include "FileScope.hpp"
#include "runtime/NumberFormatException.hpp"
#include "BuiltInFilePaths.hpp"
#include "string_helper.hpp"
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
    std::function<std::vector<Assembler::AsmInstruction>(Compiler* compiler)> onGenerateAsm
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

std::shared_ptr<BuiltInFunScope> BuiltInFunScope::INLINE_ASM=NULL;
std::shared_ptr<BuiltInFunScope> BuiltInFunScope::INT_TO_CHAR=NULL;

void BuiltInFunScope::invokeOnInterpreter(Interpreter* interpreter){
    invokeOnInterpreterFun(interpreter);
}

std::vector<Assembler::AsmInstruction> BuiltInFunScope::getGeneratedAsm(Compiler* compiler){
    return onGenerateAsm(compiler);
}

void BuiltInFunScope::addBuiltInFunctionsTo(SharedFileScope fileScope){

    if(!INLINE_ASM)
        INLINE_ASM=std::make_shared<BuiltInFunScope>(
            INLINE_ASM_NAME,
            Type::VOID,
            std::vector<std::pair<std::wstring, SharedType>>{
                {STRING_PARAM_NAME,Type::STRING},
            },
            [](Interpreter* interpreter){}
        );
    
    auto SYSCALL0=std::make_shared<BuiltInFunScope>(
        SYSCALL_NAME,
        Type::LONG,
        std::vector<std::pair<std::wstring, SharedType>>{
            {L"_RAX_",Type::LONG}, // rax
        },
        [](Interpreter* interpreter){},
        false,
        [](Compiler* compiler)->std::vector<Assembler::AsmInstruction>{
            return{
                Assembler::pop(Assembler::RAX()),
                Assembler::syscall(),
            };
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
        [](Compiler* compiler)->std::vector<Assembler::AsmInstruction>{
            return{
                Assembler::pop(Assembler::RDI()),
                Assembler::pop(Assembler::RAX()),
                Assembler::syscall(),
            };
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
        [](Compiler* compiler)->std::vector<Assembler::AsmInstruction>{
            return{
                Assembler::pop(Assembler::RSI()),
                Assembler::pop(Assembler::RDI()),
                Assembler::pop(Assembler::RAX()),
                Assembler::syscall(),
            };
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
        [](Compiler* compiler)->std::vector<Assembler::AsmInstruction>{
            return{
                Assembler::pop(Assembler::RDX()),
                Assembler::pop(Assembler::RSI()),
                Assembler::pop(Assembler::RDI()),
                Assembler::pop(Assembler::RAX()),
                Assembler::syscall(),
            };
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
        [](Compiler* compiler)->std::vector<Assembler::AsmInstruction>{
            return{
                Assembler::pop(Assembler::R10()),
                Assembler::pop(Assembler::RDX()),
                Assembler::pop(Assembler::RSI()),
                Assembler::pop(Assembler::RDI()),
                Assembler::pop(Assembler::RAX()),
                Assembler::syscall(),
            };
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
        [](Compiler* compiler)->std::vector<Assembler::AsmInstruction>{
            return{
                Assembler::pop(Assembler::R8()),
                Assembler::pop(Assembler::R10()),
                Assembler::pop(Assembler::RDX()),
                Assembler::pop(Assembler::RSI()),
                Assembler::pop(Assembler::RDI()),
                Assembler::pop(Assembler::RAX()),
                Assembler::syscall(),
            };
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
        [](Compiler* compiler)->std::vector<Assembler::AsmInstruction>{
            return{
                Assembler::pop(Assembler::R9()),
                Assembler::pop(Assembler::R8()),
                Assembler::pop(Assembler::R10()),
                Assembler::pop(Assembler::RDX()),
                Assembler::pop(Assembler::RSI()),
                Assembler::pop(Assembler::RDI()),
                Assembler::pop(Assembler::RAX()),
                Assembler::syscall(),
            };
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
        [](Compiler* compiler)->std::vector<Assembler::AsmInstruction>{
            return{
                Assembler::pop(Assembler::RDI()),
                Assembler::mov(Assembler::RSI(), Assembler::addressMov(Assembler::brk_end())),
                Assembler::lea(Assembler::RDI(), Assembler::addressLea(Assembler::RDI().value+L"+"+Assembler::RSI().value)),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"12")),
                Assembler::syscall(),
                Assembler::mov(Assembler::addressMov(Assembler::brk_end()), Assembler::RAX())
            };
        }
    );

    auto WRITE_CHAR_TO_ADDRESS=std::make_shared<BuiltInFunScope>(
        WRITE_TO_ADDRESS_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{
            {ADDRESS_PARAM_NAME,Type::LONG},
            {CHAR_PARAM_NAME,Type::CHAR},
        },
        [](Interpreter* interpreter){},
        false,
        [](Compiler* compiler)->std::vector<Assembler::AsmInstruction>{
            return{
                Assembler::mov(Assembler::RDI(Assembler::AsmInstruction::DWORD), Assembler::addressMov(Assembler::RSP())),
                Assembler::add(Assembler::RSP(), Assembler::imm(L"4")),
                Assembler::pop(Assembler::RAX()),
                Assembler::mov(Assembler::addressMov(Assembler::RAX()), Assembler::RDI(Assembler::AsmInstruction::DWORD)),
                Assembler::zero(Assembler::RAX()) // It returns 0 after a successful write
            };
        }
    );

    auto WRITE_BYTE_TO_ADDRESS=std::make_shared<BuiltInFunScope>(
        WRITE_TO_ADDRESS_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{
            {ADDRESS_PARAM_NAME,Type::LONG},
            {BYTE_PARAM_NAME,Type::BYTE},
        },
        [](Interpreter* interpreter){},
        false,
        [](Compiler* compiler)->std::vector<Assembler::AsmInstruction>{
            return{
                Assembler::mov(Assembler::RDI(Assembler::AsmInstruction::BYTE), Assembler::addressMov(Assembler::RSP())),
                Assembler::add(Assembler::RSP(), Assembler::imm(L"1")),
                Assembler::pop(Assembler::RAX()),
                Assembler::mov(Assembler::addressMov(Assembler::RAX()), Assembler::RDI(Assembler::AsmInstruction::BYTE)),
                Assembler::zero(Assembler::RAX()) // It returns 0 after a successful write
            };
        }
    );

    auto WRITE_LONG_TO_ADDRESS=std::make_shared<BuiltInFunScope>(
        WRITE_TO_ADDRESS_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{
            {ADDRESS_PARAM_NAME,Type::LONG},
            {LONG_PARAM_NAME,Type::LONG},
        },
        [](Interpreter* interpreter){},
        false,
        [](Compiler* compiler)->std::vector<Assembler::AsmInstruction>{
            return{
                Assembler::pop(Assembler::RDI()),
                Assembler::pop(Assembler::RAX()),
                Assembler::mov(Assembler::addressMov(Assembler::RAX()), Assembler::RDI()),
                Assembler::zero(Assembler::RAX()) // It returns 0 after a successful write
            };
        }
    );

    auto WRITE_ULONG_TO_ADDRESS=std::make_shared<BuiltInFunScope>(
        WRITE_TO_ADDRESS_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{
            {ADDRESS_PARAM_NAME,Type::LONG},
            {ULONG_PARAM_NAME,Type::ULONG},
        },
        [](Interpreter* interpreter){},
        false,
        [](Compiler* compiler)->std::vector<Assembler::AsmInstruction>{
            return{
                Assembler::pop(Assembler::RDI()),
                Assembler::pop(Assembler::RAX()),
                Assembler::mov(Assembler::addressMov(Assembler::RAX()), Assembler::RDI()),
                Assembler::zero(Assembler::RAX()) // It returns 0 after a successful write
            };
        }
    );

    auto READ_BYTE_FROM_ADDRESS=std::make_shared<BuiltInFunScope>(
        READ_BYTE_FROM_ADDRESS_NAME,
        Type::BYTE,
        std::vector<std::pair<std::wstring, SharedType>>{
            {ADDRESS_PARAM_NAME,Type::LONG},
        },
        [](Interpreter* interpreter){},
        false,
        [](Compiler* compiler)->std::vector<Assembler::AsmInstruction>{
            return{
                Assembler::pop(Assembler::RAX()),
                Assembler::mov(Assembler::RAX(Assembler::AsmInstruction::BYTE), Assembler::addressMov(Assembler::RAX()))
            };
        }
    );

    auto READ_INT_FROM_ADDRESS=std::make_shared<BuiltInFunScope>(
        READ_INT_FROM_ADDRESS_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{
            {ADDRESS_PARAM_NAME,Type::LONG},
        },
        [](Interpreter* interpreter){},
        false,
        [](Compiler* compiler)->std::vector<Assembler::AsmInstruction>{
            return{
                Assembler::pop(Assembler::RAX()),
                Assembler::mov(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::addressMov(Assembler::RAX()))
            };
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
        [](Compiler* compiler)->std::vector<Assembler::AsmInstruction>{
            return{
                Assembler::pop(Assembler::RAX()),
                Assembler::mov(Assembler::RAX(), Assembler::addressMov(Assembler::RAX()))
            };
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
        interpreter->AX=std::make_shared<VoidValue>();
    };

    auto PRINTLN_INVOKE_INTERPRETER_FUN=
    [](Interpreter* interpreter){
        auto msg=interpreter->top();
        ainprint(msg->toString(), true);
        interpreter->AX=std::make_shared<VoidValue>();
    };

    auto PRINT_INT=std::make_shared<BuiltInFunScope>(
        PRINT_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{{INT_PARAM_NAME,Type::INT}},
        PRINT_INVOKE_INTERPRETER_FUN
    );

    auto PRINTLN_INT=std::make_shared<BuiltInFunScope>(
        PRINTLN_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{{INT_PARAM_NAME,Type::INT}},
        PRINTLN_INVOKE_INTERPRETER_FUN
    );

    auto PRINT_UINT=std::make_shared<BuiltInFunScope>(
        PRINT_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{{UINT_PARAM_NAME,Type::UINT}},
        PRINT_INVOKE_INTERPRETER_FUN
    );

    auto PRINTLN_UINT=std::make_shared<BuiltInFunScope>(
        PRINTLN_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{{UINT_PARAM_NAME,Type::UINT}},
        PRINTLN_INVOKE_INTERPRETER_FUN
    );

    auto PRINT_LONG=std::make_shared<BuiltInFunScope>(
        PRINT_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{{LONG_PARAM_NAME,Type::LONG}},
        PRINT_INVOKE_INTERPRETER_FUN
    );

    auto PRINTLN_LONG=std::make_shared<BuiltInFunScope>(
        PRINTLN_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{{LONG_PARAM_NAME,Type::LONG}},
        PRINTLN_INVOKE_INTERPRETER_FUN
    );

    auto PRINT_ULONG=std::make_shared<BuiltInFunScope>(
        PRINT_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{{ULONG_PARAM_NAME,Type::ULONG}},
        PRINT_INVOKE_INTERPRETER_FUN
    );

    auto PRINTLN_ULONG=std::make_shared<BuiltInFunScope>(
        PRINTLN_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{{ULONG_PARAM_NAME,Type::ULONG}},
        PRINTLN_INVOKE_INTERPRETER_FUN
    );

    auto PRINT_FLOAT=std::make_shared<BuiltInFunScope>(
        PRINT_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME,Type::FLOAT}},
        PRINT_INVOKE_INTERPRETER_FUN
    );

    auto PRINTLN_FLOAT=std::make_shared<BuiltInFunScope>(
        PRINTLN_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME,Type::FLOAT}},
        PRINTLN_INVOKE_INTERPRETER_FUN
    );

    auto PRINT_DOUBLE=std::make_shared<BuiltInFunScope>(
        PRINT_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME,Type::DOUBLE}},
        PRINT_INVOKE_INTERPRETER_FUN
    );

    auto PRINTLN_DOUBLE=std::make_shared<BuiltInFunScope>(
        PRINTLN_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME,Type::DOUBLE}},
        PRINTLN_INVOKE_INTERPRETER_FUN
    );

    auto PRINT_CHAR=std::make_shared<BuiltInFunScope>(
        PRINT_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{{CHAR_PARAM_NAME,Type::CHAR}},
        PRINT_INVOKE_INTERPRETER_FUN
    );

    auto PRINTLN_CHAR=std::make_shared<BuiltInFunScope>(
        PRINTLN_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{{CHAR_PARAM_NAME,Type::CHAR}},
        PRINTLN_INVOKE_INTERPRETER_FUN
    );

    auto PRINT_STRING=std::make_shared<BuiltInFunScope>(
        PRINT_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{{STRING_PARAM_NAME,Type::STRING}},
        PRINT_INVOKE_INTERPRETER_FUN,
        false,
        [](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RAX()),
                Assembler::mov(Assembler::RDI(), Assembler::imm(L"1")),
                Assembler::lea(Assembler::RSI(), Assembler::addressLea(Assembler::RAX().value+L"+8")), // first char
                Assembler::mov(Assembler::RDX(), Assembler::addressMov(Assembler::RAX())), // size
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"1")), // sys_write
                Assembler::syscall(L"طباعة نص")
            };
        }
    );

    auto PRINTLN_STRING=std::make_shared<BuiltInFunScope>(
        PRINTLN_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{{STRING_PARAM_NAME,Type::STRING}},
        PRINTLN_INVOKE_INTERPRETER_FUN,
        false,
        [](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RAX()),
                Assembler::mov(Assembler::RDI(), Assembler::imm(L"1")),
                Assembler::lea(Assembler::RSI(), Assembler::addressLea(Assembler::RAX().value+L"+8")), // first char
                Assembler::mov(Assembler::RDX(), Assembler::addressMov(Assembler::RAX())), // size
                Assembler::lea(Assembler::R8(), Assembler::addressLea(Assembler::RAX().value+L"+8+"+Assembler::RDX().value)), // The address of the byte after the last char
                Assembler::mov(Assembler::R9(Assembler::AsmInstruction::BYTE), Assembler::addressMov(Assembler::R8())), // The byte after the last char
                Assembler::mov(Assembler::addressMov(Assembler::R8()), Assembler::imm(L"0x0a"), Assembler::AsmInstruction::BYTE), // mov '\n' to the address of that byte
                Assembler::add(Assembler::RDX(), Assembler::imm(L"1")), // add 1 for size for the '\n' char
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"1")), // sys_write
                Assembler::syscall(L"طباعة نص"),
                Assembler::mov(Assembler::addressMov(Assembler::R8()), Assembler::R9(Assembler::AsmInstruction::BYTE)) // restore the byte after the last char
            };
        }
    );

    auto PRINT_BOOL=std::make_shared<BuiltInFunScope>(
        PRINT_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{{BOOL_PARAM_NAME,Type::BOOL}},
        PRINT_INVOKE_INTERPRETER_FUN
    );

    auto PRINTLN_BOOL=std::make_shared<BuiltInFunScope>(
        PRINTLN_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{{BOOL_PARAM_NAME,Type::BOOL}},
        PRINTLN_INVOKE_INTERPRETER_FUN
    );

    auto PRINT_Void=std::make_shared<BuiltInFunScope>(
        PRINT_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{{VOID_PARAM_NAME,Type::VOID}},
        PRINT_INVOKE_INTERPRETER_FUN
    );

    auto PRINTLN_Void=std::make_shared<BuiltInFunScope>(
        PRINTLN_NAME,
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{{VOID_PARAM_NAME,Type::VOID}},
        PRINTLN_INVOKE_INTERPRETER_FUN
    );

    auto ROUND_FLOAT=std::make_shared<BuiltInFunScope>(
        ROUND_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME,Type::FLOAT}},
        [](Interpreter* interpreter){},
        false,
        [](Compiler* compiler){
            return std::vector{
                Assembler::movd(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)),
                Assembler::roundss(Assembler::XMM0(), Assembler::XMM0(), 0),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD),Assembler::XMM0())
            };
        }
    );

    auto ROUND_DOUBLE=std::make_shared<BuiltInFunScope>(
        ROUND_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME,Type::DOUBLE}},
        [](Interpreter* interpreter){},
        false,
        [](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM0(), Assembler::RAX()),
                Assembler::roundsd(Assembler::XMM0(), Assembler::XMM0(), 0),
                Assembler::movq(Assembler::RAX(),Assembler::XMM0())
            };
        }
    );

    auto FLOOR_FLOAT=std::make_shared<BuiltInFunScope>(
        FLOOR_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME,Type::FLOAT}},
        [](Interpreter* interpreter){},
        false,
        [](Compiler* compiler){
            return std::vector{
                Assembler::movd(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)),
                Assembler::roundss(Assembler::XMM0(), Assembler::XMM0(), 1),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD),Assembler::XMM0())
            };
        }
    );

    auto FLOOR_DOUBLE=std::make_shared<BuiltInFunScope>(
        FLOOR_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME,Type::DOUBLE}},
        [](Interpreter* interpreter){},
        false,
        [](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM0(), Assembler::RAX()),
                Assembler::roundsd(Assembler::XMM0(), Assembler::XMM0(), 1),
                Assembler::movq(Assembler::RAX(),Assembler::XMM0())
            };
        }
    );

    auto CEIL_FLOAT=std::make_shared<BuiltInFunScope>(
        CEILING_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME,Type::FLOAT}},
        [](Interpreter* interpreter){},
        false,
        [](Compiler* compiler){
            return std::vector{
                Assembler::movd(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)),
                Assembler::roundss(Assembler::XMM0(), Assembler::XMM0(), 2),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD),Assembler::XMM0())
            };
        }
    );

    auto CEIL_DOUBLE=std::make_shared<BuiltInFunScope>(
        CEILING_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME,Type::DOUBLE}},
        [](Interpreter* interpreter){},
        false,
        [](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM0(), Assembler::RAX()),
                Assembler::roundsd(Assembler::XMM0(), Assembler::XMM0(), 2),
                Assembler::movq(Assembler::RAX(),Assembler::XMM0())
            };
        }
    );

    auto TRUNCATE_FLOAT=std::make_shared<BuiltInFunScope>(
        TRUNCATE_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME,Type::FLOAT}},
        [](Interpreter* interpreter){},
        false,
        [](Compiler* compiler){
            return std::vector{
                Assembler::movd(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)),
                Assembler::roundss(Assembler::XMM0(), Assembler::XMM0(), 4),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD),Assembler::XMM0())
            };
        }
    );

    auto TRUNCATE_DOUBLE=std::make_shared<BuiltInFunScope>(
        TRUNCATE_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME,Type::DOUBLE}},
        [](Interpreter* interpreter){},
        false,
        [](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM0(), Assembler::RAX()),
                Assembler::roundsd(Assembler::XMM0(), Assembler::XMM0(), 4),
                Assembler::movq(Assembler::RAX(),Assembler::XMM0())
            };
        }
    );
    auto builtInFunctions={
        INLINE_ASM,
        SYSCALL0,
        SYSCALL1,
        SYSCALL2,
        SYSCALL3,
        SYSCALL4,
        SYSCALL5,
        SYSCALL6,
        BRK,
        WRITE_CHAR_TO_ADDRESS,
        WRITE_BYTE_TO_ADDRESS,
        WRITE_LONG_TO_ADDRESS,
        WRITE_ULONG_TO_ADDRESS,
        READ_BYTE_FROM_ADDRESS,
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
        PRINT_Void,PRINTLN_Void,
        ROUND_FLOAT,FLOOR_FLOAT,CEIL_FLOAT,TRUNCATE_FLOAT,
        ROUND_DOUBLE,FLOOR_DOUBLE,CEIL_DOUBLE,TRUNCATE_DOUBLE,
    };
    auto privateFunctions=fileScope->getPrivateFunctions();
    for(auto builtInFun:builtInFunctions){
        (*privateFunctions)[builtInFun->getDecl()->toString()]=builtInFun;
    }
}

void BuiltInFunScope::addBuiltInFunctionsToBuiltInClasses() {
    addBuiltInFunctionsToByteClass();
    addBuiltInFunctionsToUByteClass();
    addBuiltInFunctionsToIntClass();
    addBuiltInFunctionsToUIntClass();
    addBuiltInFunctionsToLongClass();
    addBuiltInFunctionsToULongClass();
    addBuiltInFunctionsToFloatClass();
    addBuiltInFunctionsToDoubleClass();
    addBuiltInFunctionsToBoolClass();
    addBuiltInFunctionsToCharClass();
    addBuiltInFunctionsToStringClass();
    addBuiltInFunctionsToVoidClass();
    addBuiltInFunctionsToArrayClass();
}

void BuiltInFunScope::addBuiltInFunctionsToByteClass(){

    auto classScope=std::dynamic_pointer_cast<ByteClassScope>(Type::BYTE->getClassScope());
    
    using PrimitiveType=int;

    auto LZCNT=std::make_shared<BuiltInFunScope>(
        LZCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;
            
            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFF")),
                Assembler::bsr(Assembler::RAX(), Assembler::RAX()),
                Assembler::_xor(Assembler::RAX(), Assembler::imm(L"7")),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"8")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto LOCNT=std::make_shared<BuiltInFunScope>(
        LOCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;
            
            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::_not(Assembler::RAX()),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFF")),
                Assembler::bsr(Assembler::RAX(), Assembler::RAX()),
                Assembler::_xor(Assembler::RAX(), Assembler::imm(L"7")),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"8")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto TZCNT=std::make_shared<BuiltInFunScope>(
        TZCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;
            
            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFF")),
                Assembler::bsf(Assembler::RAX(), Assembler::RAX()),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"8")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto TOCNT=std::make_shared<BuiltInFunScope>(
        TOCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;

            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::_not(Assembler::RAX()),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFF")),
                Assembler::bsf(Assembler::RAX(), Assembler::RAX()),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"8")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto PLUS_BYTE=getPlusFun<PrimitiveType, ByteValue, ByteValue>(
        classScope,
        Type::BYTE,
        BYTE_PARAM_NAME,
        Type::BYTE
    );

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

    auto PLUS_FLOAT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::PLUS_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME, Type::FLOAT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to float
                Assembler::cbw(),
                Assembler::cwde(),
                Assembler::cvtsi2ss(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::addss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto PLUS_DOUBLE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::PLUS_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME, Type::DOUBLE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to double
                Assembler::cbw(),
                Assembler::cwde(),
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::addsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto MINUS_BYTE=getMinusFun<PrimitiveType, ByteValue, ByteValue>(
        classScope,
        Type::BYTE,
        BYTE_PARAM_NAME,
        Type::BYTE
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

    auto MINUS_FLOAT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::MINUS_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME, Type::FLOAT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to float
                Assembler::cbw(),
                Assembler::cwde(),
                Assembler::cvtsi2ss(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::subss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto MINUS_DOUBLE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::MINUS_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME, Type::DOUBLE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to double
                Assembler::cbw(),
                Assembler::cwde(),
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::subsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto TIMES_BYTE=getTimesFun<PrimitiveType, ByteValue, ByteValue>(
        classScope,
        Type::BYTE,
        BYTE_PARAM_NAME,
        Type::BYTE
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

    auto TIMES_FLOAT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::TIMES_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME, Type::FLOAT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to float
                Assembler::cbw(),
                Assembler::cwde(),
                Assembler::cvtsi2ss(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::mulss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto TIMES_DOUBLE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::TIMES_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME, Type::DOUBLE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to double
                Assembler::cbw(),
                Assembler::cwde(),
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::mulsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto DIV_BYTE=getDivFun<PrimitiveType, ByteValue, ByteValue>(
        classScope,
        Type::BYTE,
        BYTE_PARAM_NAME,
        Type::BYTE
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

    auto DIV_FLOAT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::DIV_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME, Type::FLOAT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to float
                Assembler::cbw(),
                Assembler::cwde(),
                Assembler::cvtsi2ss(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::divss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto DIV_DOUBLE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::DIV_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME, Type::DOUBLE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to double
                Assembler::cbw(),
                Assembler::cwde(),
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::divsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto MOD_BYTE=getModFun<PrimitiveType, ByteValue, ByteValue>(
        classScope,
        Type::BYTE,
        BYTE_PARAM_NAME,
        Type::BYTE
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

    auto COMPARE_TO_BYTE=getCompareToFun<PrimitiveType, ByteValue>(
        classScope,
        BYTE_PARAM_NAME,
        Type::BYTE
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

    auto UNARY_PLUS=getUnaryPlusFun<PrimitiveType,ByteValue>(classScope,Type::BYTE);

    auto UNARY_MINUS=getUnaryMinusFun<PrimitiveType,ByteValue>(classScope,Type::BYTE);

    auto INC=getIncFun<PrimitiveType,ByteValue>(classScope,Type::BYTE);

    auto DEC=getDecFun<PrimitiveType,ByteValue>(classScope,Type::BYTE);

    auto TO_BYTE=getToByteFun<PrimitiveType>(classScope);

    auto TO_UBYTE=getToUByteFun<PrimitiveType>(classScope);

    auto TO_INT=std::make_shared<BuiltInFunScope>(
        TO_INT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::cbw(),
                Assembler::cwde()
            };
        }
    );

    auto TO_UINT=std::make_shared<BuiltInFunScope>(
        TO_UINT_NAME,
        Type::UINT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::cbw(),
                Assembler::cwde()
            };
        }
    );

    auto TO_LONG=std::make_shared<BuiltInFunScope>(
        TO_LONG_NAME,
        Type::LONG,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::cbw(),
                Assembler::cwde(),
                Assembler::cdqe()
            };
        }
    );

    auto TO_ULONG=std::make_shared<BuiltInFunScope>(
        TO_ULONG_NAME,
        Type::ULONG,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::cbw(),
                Assembler::cwde(),
                Assembler::cdqe()
            };
        }
    );

    auto TO_FLOAT=std::make_shared<BuiltInFunScope>(
        TO_FLOAT_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::cbw(),
                Assembler::cwde(),
                Assembler::cvtsi2ss(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto TO_DOUBLE=std::make_shared<BuiltInFunScope>(
        TO_DOUBLE_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::cbw(),
                Assembler::cwde(),
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto TO_STRING=getToStringFun<PrimitiveType>(classScope);

    auto SHR=getShrFun<PrimitiveType,ByteValue>(classScope,Type::BYTE);

    auto SHL=getShlFun<PrimitiveType,ByteValue>(classScope,Type::BYTE);

    auto BIT_AND=getBitAndFun<PrimitiveType,ByteValue>(
        classScope,
        Type::BYTE,
        BYTE_PARAM_NAME
    );

    auto XOR=getXorFun<PrimitiveType,ByteValue>(
        classScope,
        Type::BYTE,
        BYTE_PARAM_NAME
    );

    auto BIT_OR=getBitOrFun<PrimitiveType,ByteValue>(
        classScope,
        Type::BYTE,
        BYTE_PARAM_NAME
    );

    auto BIT_NOT=getBitNotFun<PrimitiveType,ByteValue>(
        classScope,
        Type::BYTE
    );

    auto funs={
        LZCNT,LOCNT,TZCNT,TOCNT,
        PLUS_BYTE,PLUS_INT,PLUS_LONG,PLUS_FLOAT,PLUS_DOUBLE,
        MINUS_BYTE,MINUS_INT,MINUS_LONG,MINUS_FLOAT,MINUS_DOUBLE,
        TIMES_BYTE,TIMES_INT,TIMES_LONG,TIMES_FLOAT,TIMES_DOUBLE,
        DIV_BYTE,DIV_INT,DIV_LONG,DIV_FLOAT,DIV_DOUBLE,
        MOD_BYTE,MOD_INT,MOD_LONG,
        COMPARE_TO_BYTE,COMPARE_TO_INT,COMPARE_TO_LONG,COMPARE_TO_FLOAT,COMPARE_TO_DOUBLE,
        EQUALS,
        UNARY_PLUS,UNARY_MINUS,
        INC,DEC,
        TO_BYTE,TO_UBYTE,TO_INT,TO_UINT,TO_LONG,TO_ULONG,
        TO_FLOAT,TO_DOUBLE,TO_STRING,
        SHR,SHL,BIT_AND,XOR,BIT_OR,BIT_NOT
    };

    auto publicFuns=classScope->getPublicFunctions();
    for(auto fun:funs){
        (*publicFuns)[fun->getDecl()->toString()]=fun;
    }
    
}

void BuiltInFunScope::addBuiltInFunctionsToUByteClass(){

    auto classScope=std::dynamic_pointer_cast<UByteClassScope>(Type::UBYTE->getClassScope());
    
    using PrimitiveType=unsigned int;


    auto LZCNT=std::make_shared<BuiltInFunScope>(
        LZCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;
            
            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFF")),
                Assembler::bsr(Assembler::RAX(), Assembler::RAX()),
                Assembler::_xor(Assembler::RAX(), Assembler::imm(L"7")),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"8")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto LOCNT=std::make_shared<BuiltInFunScope>(
        LOCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;
            
            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::_not(Assembler::RAX()),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFF")),
                Assembler::bsr(Assembler::RAX(), Assembler::RAX()),
                Assembler::_xor(Assembler::RAX(), Assembler::imm(L"7")),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"8")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto TZCNT=std::make_shared<BuiltInFunScope>(
        TZCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;
            
            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFF")),
                Assembler::bsf(Assembler::RAX(), Assembler::RAX()),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"8")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto TOCNT=std::make_shared<BuiltInFunScope>(
        TOCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;

            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::_not(Assembler::RAX()),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFF")),
                Assembler::bsf(Assembler::RAX(), Assembler::RAX()),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"8")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto PLUS_UBYTE=getPlusFun<PrimitiveType, UByteValue, UByteValue>(
        classScope,
        Type::UBYTE,
        UBYTE_PARAM_NAME,
        Type::UBYTE
    );

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

    auto MINUS_UBYTE=getMinusFun<PrimitiveType, UByteValue, UByteValue>(
        classScope,
        Type::UBYTE,
        UBYTE_PARAM_NAME,
        Type::UBYTE
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

    auto TIMES_UBYTE=getTimesFun<PrimitiveType, UByteValue, UByteValue>(
        classScope,
        Type::UBYTE,
        UBYTE_PARAM_NAME,
        Type::UBYTE
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

    auto DIV_UBYTE=getDivFun<PrimitiveType, UByteValue, UByteValue>(
        classScope,
        Type::UBYTE,
        UBYTE_PARAM_NAME,
        Type::UBYTE
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

    auto MOD_UBYTE=getTimesFun<PrimitiveType, UByteValue, UByteValue>(
        classScope,
        Type::UBYTE,
        UBYTE_PARAM_NAME,
        Type::UBYTE
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

    auto COMPARE_TO_UBYTE=getCompareToFun<PrimitiveType, ByteValue>(
        classScope,
        UBYTE_PARAM_NAME,
        Type::UBYTE
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
        UBYTE_PARAM_NAME,
        Type::UBYTE
    );

    auto INC=getIncFun<PrimitiveType,UIntValue>(classScope,Type::UBYTE);

    auto DEC=getDecFun<PrimitiveType,UIntValue>(classScope,Type::UBYTE);

    auto TO_BYTE=getToByteFun<PrimitiveType>(classScope);

    auto TO_UBYTE=getToUByteFun<PrimitiveType>(classScope);

    auto TO_INT=std::make_shared<BuiltInFunScope>(
        TO_INT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFF"))
            };
        }
    );

    auto TO_UINT=std::make_shared<BuiltInFunScope>(
        TO_UINT_NAME,
        Type::UINT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFF"))
            };
        }
    );

    auto TO_LONG=std::make_shared<BuiltInFunScope>(
        TO_LONG_NAME,
        Type::LONG,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFF"))
            };
        }
    );

    auto TO_ULONG=std::make_shared<BuiltInFunScope>(
        TO_ULONG_NAME,
        Type::ULONG,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFF"))
            };
        }
    );

    auto TO_FLOAT=std::make_shared<BuiltInFunScope>(
        TO_FLOAT_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFF")),
                Assembler::cvtsi2ss(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto TO_DOUBLE=std::make_shared<BuiltInFunScope>(
        TO_DOUBLE_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFF")),
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto TO_STRING=getToStringFun<PrimitiveType>(classScope);

    auto SHR=getShrFun<PrimitiveType,UByteValue>(classScope,Type::UBYTE);

    auto SHL=getShlFun<PrimitiveType,UByteValue>(classScope,Type::UBYTE);

    auto BIT_AND=getBitAndFun<PrimitiveType,UByteValue>(
        classScope,
        Type::UBYTE,
        UBYTE_PARAM_NAME
    );

    auto XOR=getXorFun<PrimitiveType,UByteValue>(
        classScope,
        Type::UBYTE,
        UBYTE_PARAM_NAME
    );

    auto BIT_OR=getBitOrFun<PrimitiveType,UByteValue>(
        classScope,
        Type::UBYTE,
        UBYTE_PARAM_NAME
    );

    auto BIT_NOT=getBitNotFun<PrimitiveType,UByteValue>(
        classScope,
        Type::UBYTE
    );

    auto funs={
        LZCNT,LOCNT,TZCNT,TOCNT,
        PLUS_UBYTE,PLUS_UINT,PLUS_ULONG,
        MINUS_UBYTE,MINUS_UINT,MINUS_ULONG,
        TIMES_UBYTE,TIMES_UINT,TIMES_ULONG,
        DIV_UBYTE,DIV_UINT,DIV_ULONG,
        MOD_UBYTE,MOD_UINT,MOD_ULONG,
        COMPARE_TO_UBYTE,COMPARE_TO_UINT,COMPARE_TO_ULONG,
        EQUALS,
        INC,DEC,
        TO_BYTE,TO_UBYTE,TO_INT,TO_UINT,TO_LONG,TO_ULONG,
        TO_FLOAT,TO_DOUBLE,TO_STRING,
        SHR,SHL,BIT_AND,XOR,BIT_OR,BIT_NOT
    };

    auto publicFuns=classScope->getPublicFunctions();
    for(auto fun:funs){
        (*publicFuns)[fun->getDecl()->toString()]=fun;
    }
    
}

void BuiltInFunScope::addBuiltInFunctionsToIntClass(){

    auto classScope=std::dynamic_pointer_cast<IntClassScope>(Type::INT->getClassScope());
    
    using PrimitiveType=int;

    auto LZCNT=std::make_shared<BuiltInFunScope>(
        LZCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;
            
            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFFFF")),
                Assembler::bsr(Assembler::RAX(), Assembler::RAX()),
                Assembler::_xor(Assembler::RAX(), Assembler::imm(L"31")),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"32")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto LOCNT=std::make_shared<BuiltInFunScope>(
        LOCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;
            
            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::_not(Assembler::RAX()),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFFFF")),
                Assembler::bsr(Assembler::RAX(), Assembler::RAX()),
                Assembler::_xor(Assembler::RAX(), Assembler::imm(L"31")),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"32")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto TZCNT=std::make_shared<BuiltInFunScope>(
        TZCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;
            
            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFFFF")),
                Assembler::bsf(Assembler::RAX(), Assembler::RAX()),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"32")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto TOCNT=std::make_shared<BuiltInFunScope>(
        TOCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;

            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::_not(Assembler::RAX()),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFFFF")),
                Assembler::bsf(Assembler::RAX(), Assembler::RAX()),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"32")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto PLUS_BYTE=getPlusFun<PrimitiveType, ByteValue, IntValue>(
        classScope,
        Type::INT,
        BYTE_PARAM_NAME,
        Type::BYTE
    );

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

    auto PLUS_FLOAT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::PLUS_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME, Type::FLOAT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to float
                Assembler::cvtsi2ss(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::addss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto PLUS_DOUBLE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::PLUS_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME, Type::DOUBLE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to double
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::addsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto MINUS_BYTE=getMinusFun<PrimitiveType, ByteValue, IntValue>(
        classScope,
        Type::INT,
        BYTE_PARAM_NAME,
        Type::BYTE
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

    auto MINUS_FLOAT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::MINUS_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME, Type::FLOAT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to float
                Assembler::cvtsi2ss(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::subss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto MINUS_DOUBLE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::MINUS_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME, Type::DOUBLE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to double
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::subsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );


    auto TIMES_BYTE=getTimesFun<PrimitiveType, ByteValue, IntValue>(
        classScope,
        Type::INT,
        BYTE_PARAM_NAME,
        Type::BYTE
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

    auto TIMES_FLOAT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::TIMES_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME, Type::FLOAT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to float
                Assembler::cvtsi2ss(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::mulss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto TIMES_DOUBLE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::TIMES_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME, Type::DOUBLE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to double
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::mulsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto DIV_BYTE=getDivFun<PrimitiveType, ByteValue, IntValue>(
        classScope,
        Type::INT,
        BYTE_PARAM_NAME,
        Type::BYTE
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

    auto DIV_FLOAT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::DIV_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME, Type::FLOAT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to float
                Assembler::cvtsi2ss(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::divss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto DIV_DOUBLE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::DIV_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME, Type::DOUBLE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to double
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::divsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto MOD_BYTE=getModFun<PrimitiveType, ByteValue, IntValue>(
        classScope,
        Type::INT,
        BYTE_PARAM_NAME,
        Type::BYTE
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

    auto COMPARE_TO_BYTE=getCompareToFun<PrimitiveType, ByteValue>(
        classScope,
        BYTE_PARAM_NAME,
        Type::BYTE
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

    auto TO_BYTE=getToByteFun<PrimitiveType>(classScope);

    auto TO_UBYTE=getToUByteFun<PrimitiveType>(classScope);

    auto TO_INT=getToIntFun<PrimitiveType>(classScope);

    auto TO_UINT=getToUIntFun<PrimitiveType>(classScope);

    auto TO_LONG=std::make_shared<BuiltInFunScope>(
        TO_LONG_NAME,
        Type::LONG,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::cdqe()
            };
        }
    );

    auto TO_ULONG=std::make_shared<BuiltInFunScope>(
        TO_ULONG_NAME,
        Type::ULONG,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::cdqe()
            };
        }
    );

    auto TO_FLOAT=std::make_shared<BuiltInFunScope>(
        TO_FLOAT_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::cvtsi2ss(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto TO_DOUBLE=std::make_shared<BuiltInFunScope>(
        TO_DOUBLE_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto TO_STRING=getToStringFun<PrimitiveType>(classScope);

    if(!INT_TO_CHAR)
        INT_TO_CHAR=std::make_shared<BuiltInFunScope>(
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
            },
            false,
            [=](Compiler* compiler){
                return std::vector{
                    Assembler::ret()
                };
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

    auto BIN_REPRESENTATION=std::make_shared<BuiltInFunScope>(
        BIN_REPRESENTATION_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){}
    );

    auto funs={
        LZCNT,LOCNT,TZCNT,TOCNT,
        PLUS_BYTE,PLUS_INT,PLUS_LONG,PLUS_FLOAT,PLUS_DOUBLE,
        MINUS_BYTE,MINUS_INT,MINUS_LONG,MINUS_FLOAT,MINUS_DOUBLE,
        TIMES_BYTE,TIMES_INT,TIMES_LONG,TIMES_FLOAT,TIMES_DOUBLE,
        DIV_BYTE,DIV_INT,DIV_LONG,DIV_FLOAT,DIV_DOUBLE,
        MOD_BYTE,MOD_INT,MOD_LONG,
        COMPARE_TO_BYTE,COMPARE_TO_INT,COMPARE_TO_LONG,COMPARE_TO_FLOAT,COMPARE_TO_DOUBLE,
        EQUALS,
        UNARY_PLUS,UNARY_MINUS,
        INC,DEC,
        TO_BYTE,TO_UBYTE,TO_INT,TO_UINT,TO_LONG,TO_ULONG,
        TO_FLOAT,TO_DOUBLE,TO_STRING,INT_TO_CHAR,
        SHR,SHL,BIT_AND,XOR,BIT_OR,BIT_NOT,
        BIN_REPRESENTATION,
    };

    auto publicFuns=classScope->getPublicFunctions();
    for(auto fun:funs){
        (*publicFuns)[fun->getDecl()->toString()]=fun;
    }

}

void BuiltInFunScope::addBuiltInFunctionsToUIntClass(){

    auto classScope=std::dynamic_pointer_cast<UIntClassScope>(Type::UINT->getClassScope());
    
    using PrimitiveType=unsigned int;

    auto LZCNT=std::make_shared<BuiltInFunScope>(
        LZCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;
            
            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFFFF")),
                Assembler::bsr(Assembler::RAX(), Assembler::RAX()),
                Assembler::_xor(Assembler::RAX(), Assembler::imm(L"31")),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"32")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto LOCNT=std::make_shared<BuiltInFunScope>(
        LOCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;
            
            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::_not(Assembler::RAX()),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFFFF")),
                Assembler::bsr(Assembler::RAX(), Assembler::RAX()),
                Assembler::_xor(Assembler::RAX(), Assembler::imm(L"31")),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"32")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto TZCNT=std::make_shared<BuiltInFunScope>(
        TZCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;
            
            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFFFF")),
                Assembler::bsf(Assembler::RAX(), Assembler::RAX()),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"32")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto TOCNT=std::make_shared<BuiltInFunScope>(
        TOCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;

            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::_not(Assembler::RAX()),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFFFF")),
                Assembler::bsf(Assembler::RAX(), Assembler::RAX()),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"32")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto PLUS_UBYTE=getPlusFun<PrimitiveType, UByteValue, UIntValue>(
        classScope,
        Type::UINT,
        UBYTE_PARAM_NAME,
        Type::UBYTE
    );

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

    auto MINUS_UBYTE=getMinusFun<PrimitiveType, UByteValue, UIntValue>(
        classScope,
        Type::UINT,
        UBYTE_PARAM_NAME,
        Type::UBYTE
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

    auto TIMES_UBYTE=getTimesFun<PrimitiveType, UByteValue, UIntValue>(
        classScope,
        Type::UINT,
        UBYTE_PARAM_NAME,
        Type::UBYTE
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

    auto DIV_UBYTE=getDivFun<PrimitiveType, UByteValue, UIntValue>(
        classScope,
        Type::UINT,
        UBYTE_PARAM_NAME,
        Type::UBYTE
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

    auto MOD_UBYTE=getTimesFun<PrimitiveType, UByteValue, UIntValue>(
        classScope,
        Type::UINT,
        UBYTE_PARAM_NAME,
        Type::UBYTE
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

    auto COMPARE_TO_UBYTE=getCompareToFun<PrimitiveType, UByteValue>(
        classScope,
        UBYTE_PARAM_NAME,
        Type::UBYTE
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

    auto TO_BYTE=getToByteFun<PrimitiveType>(classScope);

    auto TO_UBYTE=getToUByteFun<PrimitiveType>(classScope);

    auto TO_INT=getToIntFun<PrimitiveType>(classScope);

    auto TO_UINT=getToUIntFun<PrimitiveType>(classScope);

    auto TO_LONG=std::make_shared<BuiltInFunScope>(
        TO_LONG_NAME,
        Type::LONG,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::mov(
                    Assembler::RAX(Assembler::AsmInstruction::DWORD),
                    Assembler::RAX(Assembler::AsmInstruction::DWORD)
                )
            };
        }
    );

    auto TO_ULONG=std::make_shared<BuiltInFunScope>(
        TO_ULONG_NAME,
        Type::ULONG,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::mov(
                    Assembler::RAX(Assembler::AsmInstruction::DWORD),
                    Assembler::RAX(Assembler::AsmInstruction::DWORD)
                )
            };
        }
    );

    auto TO_FLOAT=std::make_shared<BuiltInFunScope>(
        TO_FLOAT_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::cvtsi2ss(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto TO_DOUBLE=std::make_shared<BuiltInFunScope>(
        TO_DOUBLE_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

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
        LZCNT,LOCNT,TZCNT,TOCNT,
        PLUS_UBYTE,PLUS_UINT,PLUS_ULONG,
        MINUS_UBYTE,MINUS_UINT,MINUS_ULONG,
        TIMES_UBYTE,TIMES_UINT,TIMES_ULONG,
        DIV_UBYTE,DIV_UINT,DIV_ULONG,
        MOD_UBYTE,MOD_UINT,MOD_ULONG,
        COMPARE_TO_UBYTE,COMPARE_TO_UINT,COMPARE_TO_ULONG,
        EQUALS,
        INC,DEC,
        TO_BYTE,TO_UBYTE,TO_INT,TO_UINT,TO_LONG,TO_ULONG,
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

    auto LZCNT=std::make_shared<BuiltInFunScope>(
        LZCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;
            
            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::bsr(Assembler::RAX(), Assembler::RAX()),
                Assembler::_xor(Assembler::RAX(), Assembler::imm(L"63")),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"64")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto LOCNT=std::make_shared<BuiltInFunScope>(
        LOCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;
            
            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::_not(Assembler::RAX()),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::bsr(Assembler::RAX(), Assembler::RAX()),
                Assembler::_xor(Assembler::RAX(), Assembler::imm(L"63")),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"64")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto TZCNT=std::make_shared<BuiltInFunScope>(
        TZCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;
            
            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::bsf(Assembler::RAX(), Assembler::RAX()),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"64")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto TOCNT=std::make_shared<BuiltInFunScope>(
        TOCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;

            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::_not(Assembler::RAX()),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::bsf(Assembler::RAX(), Assembler::RAX()),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"64")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto PLUS_BYTE=getPlusFun<PrimitiveType, ByteValue, LongValue>(
        classScope,
        Type::LONG,
        BYTE_PARAM_NAME,
        Type::BYTE
    );

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

    auto PLUS_FLOAT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::PLUS_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME, Type::FLOAT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to float
                Assembler::cvtsi2ss(Assembler::XMM0(), Assembler::RAX()),
                Assembler::addss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto PLUS_DOUBLE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::PLUS_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME, Type::DOUBLE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to double
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RAX()),
                Assembler::addsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto MINUS_BYTE=getMinusFun<PrimitiveType, ByteValue, LongValue>(
        classScope,
        Type::LONG,
        BYTE_PARAM_NAME,
        Type::BYTE
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

    auto MINUS_FLOAT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::MINUS_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME, Type::FLOAT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to float
                Assembler::cvtsi2ss(Assembler::XMM0(), Assembler::RAX()),
                Assembler::subss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto MINUS_DOUBLE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::MINUS_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME, Type::DOUBLE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to double
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RAX()),
                Assembler::subsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto TIMES_BYTE=getTimesFun<PrimitiveType, ByteValue, LongValue>(
        classScope,
        Type::LONG,
        BYTE_PARAM_NAME,
        Type::BYTE
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

    auto TIMES_FLOAT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::TIMES_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME, Type::FLOAT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to float
                Assembler::cvtsi2ss(Assembler::XMM0(), Assembler::RAX()),
                Assembler::mulss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto TIMES_DOUBLE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::TIMES_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME, Type::DOUBLE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to double
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RAX()),
                Assembler::mulsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto DIV_BYTE=getDivFun<PrimitiveType, ByteValue, LongValue>(
        classScope,
        Type::LONG,
        BYTE_PARAM_NAME,
        Type::BYTE
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

    auto DIV_FLOAT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::DIV_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME, Type::FLOAT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to float
                Assembler::cvtsi2ss(Assembler::XMM0(), Assembler::RAX()),
                Assembler::divss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto DIV_DOUBLE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::DIV_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME, Type::DOUBLE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // 2nd arg
                Assembler::pop(Assembler::RAX()), // 1st arg
                // convert 1st arg to double
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RAX()),
                Assembler::divsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto MOD_BYTE=getModFun<PrimitiveType, ByteValue, LongValue>(
        classScope,
        Type::LONG,
        BYTE_PARAM_NAME,
        Type::BYTE
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

    auto COMPARE_TO_BYTE=getCompareToFun<PrimitiveType, ByteValue>(
        classScope,
        BYTE_PARAM_NAME,
        Type::BYTE
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

    auto TO_BYTE=getToByteFun<PrimitiveType>(classScope);

    auto TO_UBYTE=getToUByteFun<PrimitiveType>(classScope);

    auto TO_INT=getToIntFun<PrimitiveType>(classScope);

    auto TO_UINT=getToUIntFun<PrimitiveType>(classScope);

    auto TO_LONG=getToLongFun<PrimitiveType>(classScope);

    auto TO_ULONG=getToULongFun<PrimitiveType>(classScope);

    auto TO_FLOAT=std::make_shared<BuiltInFunScope>(
        TO_FLOAT_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::cvtsi2ss(Assembler::XMM0(), Assembler::RAX()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto TO_DOUBLE=std::make_shared<BuiltInFunScope>(
        TO_DOUBLE_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RAX()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

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

    auto BIN_REPRESENTATION=std::make_shared<BuiltInFunScope>(
        BIN_REPRESENTATION_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){}
    );

    auto funs={
        LZCNT,LOCNT,TZCNT,TOCNT,
        PLUS_BYTE,PLUS_INT,PLUS_LONG,PLUS_FLOAT,PLUS_DOUBLE,
        MINUS_BYTE,MINUS_INT,MINUS_LONG,MINUS_FLOAT,MINUS_DOUBLE,
        TIMES_BYTE,TIMES_INT,TIMES_LONG,TIMES_FLOAT,TIMES_DOUBLE,
        DIV_BYTE,DIV_INT,DIV_LONG,DIV_FLOAT,DIV_DOUBLE,
        MOD_BYTE,MOD_INT,MOD_LONG,
        COMPARE_TO_BYTE,COMPARE_TO_INT,COMPARE_TO_LONG,COMPARE_TO_FLOAT,COMPARE_TO_DOUBLE,
        EQUALS,
        UNARY_PLUS,UNARY_MINUS,
        INC,DEC,
        TO_BYTE,TO_UBYTE,TO_INT,TO_UINT,TO_LONG,TO_ULONG,
        TO_FLOAT,TO_DOUBLE,TO_STRING,
        SHR,SHL,BIT_AND,XOR,BIT_OR,BIT_NOT,
        BIN_REPRESENTATION,
    };

    auto publicFuns=classScope->getPublicFunctions();
    for(auto fun:funs){
        (*publicFuns)[fun->getDecl()->toString()]=fun;
    }

}

void BuiltInFunScope::addBuiltInFunctionsToULongClass(){

    auto classScope=std::dynamic_pointer_cast<ULongClassScope>(Type::ULONG->getClassScope());
    
    using PrimitiveType=unsigned long long;

    auto LZCNT=std::make_shared<BuiltInFunScope>(
        LZCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;
            
            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::bsr(Assembler::RAX(), Assembler::RAX()),
                Assembler::_xor(Assembler::RAX(), Assembler::imm(L"63")),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"64")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto LOCNT=std::make_shared<BuiltInFunScope>(
        LOCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;
            
            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::_not(Assembler::RAX()),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::bsr(Assembler::RAX(), Assembler::RAX()),
                Assembler::_xor(Assembler::RAX(), Assembler::imm(L"63")),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"64")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto TZCNT=std::make_shared<BuiltInFunScope>(
        TZCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;
            
            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::bsf(Assembler::RAX(), Assembler::RAX()),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"64")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto TOCNT=std::make_shared<BuiltInFunScope>(
        TOCNT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto ifNumStr=std::to_wstring(++compiler->currentIfLabelsSize);
            auto ifLabelStr=L"if"+ifNumStr;
            auto elseLabelStr=L"else"+ifNumStr;
            auto endLabelStr=L"end"+ifNumStr;

            return std::vector{
                Assembler::localLabel(ifLabelStr),
                Assembler::_not(Assembler::RAX()),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jz(Assembler::label(L"."+elseLabelStr)),
                Assembler::bsf(Assembler::RAX(), Assembler::RAX()),
                Assembler::jmp(Assembler::label(L"."+endLabelStr)),
                Assembler::localLabel(elseLabelStr),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"64")),
                Assembler::localLabel(endLabelStr)
            };
        }
    );

    auto PLUS_UBYTE=getPlusFun<PrimitiveType, UByteValue, ULongValue>(
        classScope,
        Type::ULONG,
        UBYTE_PARAM_NAME,
        Type::UBYTE
    );

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

    auto MINUS_UBYTE=getMinusFun<PrimitiveType, UByteValue, ULongValue>(
        classScope,
        Type::ULONG,
        UBYTE_PARAM_NAME,
        Type::UBYTE
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

    auto TIMES_UBYTE=getTimesFun<PrimitiveType, UByteValue, ULongValue>(
        classScope,
        Type::ULONG,
        UBYTE_PARAM_NAME,
        Type::UBYTE
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

    auto DIV_UBYTE=getDivFun<PrimitiveType, UByteValue, ULongValue>(
        classScope,
        Type::ULONG,
        UBYTE_PARAM_NAME,
        Type::UBYTE
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

    auto MOD_UBYTE=getTimesFun<PrimitiveType, UByteValue, ULongValue>(
        classScope,
        Type::ULONG,
        UBYTE_PARAM_NAME,
        Type::UBYTE
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

    auto COMPARE_TO_UBYTE=getCompareToFun<PrimitiveType, UByteValue>(
        classScope,
        UBYTE_PARAM_NAME,
        Type::UBYTE
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

    auto TO_BYTE=getToByteFun<PrimitiveType>(classScope);

    auto TO_UBYTE=getToUByteFun<PrimitiveType>(classScope);

    auto TO_INT=getToIntFun<PrimitiveType>(classScope);

    auto TO_UINT=getToUIntFun<PrimitiveType>(classScope);

    auto TO_LONG=getToLongFun<PrimitiveType>(classScope);

    auto TO_ULONG=getToULongFun<PrimitiveType>(classScope);

    auto TO_FLOAT=std::make_shared<BuiltInFunScope>(
        TO_FLOAT_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::cvtsi2ss(Assembler::XMM0(), Assembler::RAX()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto TO_DOUBLE=std::make_shared<BuiltInFunScope>(
        TO_DOUBLE_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RAX()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

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
        LZCNT,LOCNT,TZCNT,TOCNT,
        PLUS_UBYTE,PLUS_UINT,PLUS_ULONG,
        MINUS_UBYTE,MINUS_UINT,MINUS_ULONG,
        TIMES_UBYTE,TIMES_UINT,TIMES_ULONG,
        DIV_UBYTE,DIV_UINT,DIV_ULONG,
        MOD_UBYTE,MOD_UINT,MOD_ULONG,
        COMPARE_TO_UBYTE,COMPARE_TO_UINT,COMPARE_TO_ULONG,
        EQUALS,
        INC,DEC,
        TO_BYTE,TO_UBYTE,TO_INT,TO_UINT,TO_LONG,TO_ULONG,
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

    auto PLUS_BYTE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::PLUS_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{BYTE_PARAM_NAME, Type::BYTE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movd(Assembler::XMM0(), Assembler::RCX(Assembler::AsmInstruction::DWORD)), // first arg
                // convert 2nd arg to float
                Assembler::cbw(),
                Assembler::cwde(),
                Assembler::cvtsi2ss(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::addss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto PLUS_INT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::PLUS_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{INT_PARAM_NAME, Type::INT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movd(Assembler::XMM0(), Assembler::RCX(Assembler::AsmInstruction::DWORD)), // first arg
                // convert 2nd arg to float
                Assembler::cvtsi2ss(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::addss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto PLUS_LONG=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::PLUS_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{LONG_PARAM_NAME, Type::LONG}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movd(Assembler::XMM0(), Assembler::RCX(Assembler::AsmInstruction::DWORD)), // first arg
                // convert 2nd arg to float
                Assembler::cvtsi2ss(Assembler::XMM1(), Assembler::RAX()),
                Assembler::addss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto PLUS_FLOAT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::PLUS_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME, Type::FLOAT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movd(Assembler::XMM0(), Assembler::RCX(Assembler::AsmInstruction::DWORD)), // first arg
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // second arg
                Assembler::addss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0()),
            };
        }
    );

    auto PLUS_DOUBLE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::PLUS_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME, Type::DOUBLE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                // convert 1st arg to double
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RCX(Assembler::AsmInstruction::DWORD)), // for optimization
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // second arg
                Assembler::addsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto MINUS_BYTE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::MINUS_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{BYTE_PARAM_NAME, Type::BYTE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movd(Assembler::XMM0(), Assembler::RCX(Assembler::AsmInstruction::DWORD)), // first arg
                // convert 2nd arg to float
                Assembler::cbw(),
                Assembler::cwde(),
                Assembler::cvtsi2ss(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::subss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto MINUS_INT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::MINUS_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{INT_PARAM_NAME, Type::INT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movd(Assembler::XMM0(), Assembler::RCX(Assembler::AsmInstruction::DWORD)), // first arg
                // convert 2nd arg to float
                Assembler::cvtsi2ss(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::subss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto MINUS_LONG=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::MINUS_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{LONG_PARAM_NAME, Type::LONG}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movd(Assembler::XMM0(), Assembler::RCX(Assembler::AsmInstruction::DWORD)), // first arg
                // convert 2nd arg to float
                Assembler::cvtsi2ss(Assembler::XMM1(), Assembler::RAX()),
                Assembler::subss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto MINUS_FLOAT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::MINUS_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME, Type::FLOAT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movd(Assembler::XMM0(), Assembler::RCX(Assembler::AsmInstruction::DWORD)), // first arg
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // second arg
                Assembler::subss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0()),
            };
        }
    );

    auto MINUS_DOUBLE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::MINUS_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME, Type::DOUBLE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                // convert 1st arg to double
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RCX(Assembler::AsmInstruction::DWORD)), // for optimization
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // second arg
                Assembler::subsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto TIMES_BYTE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::TIMES_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{BYTE_PARAM_NAME, Type::BYTE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movd(Assembler::XMM0(), Assembler::RCX(Assembler::AsmInstruction::DWORD)), // first arg
                // convert 2nd arg to float
                Assembler::cbw(),
                Assembler::cwde(),
                Assembler::cvtsi2ss(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::mulss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto TIMES_INT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::TIMES_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{INT_PARAM_NAME, Type::INT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movd(Assembler::XMM0(), Assembler::RCX(Assembler::AsmInstruction::DWORD)), // first arg
                // convert 2nd arg to float
                Assembler::cvtsi2ss(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::mulss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto TIMES_LONG=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::TIMES_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{LONG_PARAM_NAME, Type::LONG}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movd(Assembler::XMM0(), Assembler::RCX(Assembler::AsmInstruction::DWORD)), // first arg
                // convert 2nd arg to float
                Assembler::cvtsi2ss(Assembler::XMM1(), Assembler::RAX()),
                Assembler::mulss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto TIMES_FLOAT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::TIMES_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME, Type::FLOAT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movd(Assembler::XMM0(), Assembler::RCX(Assembler::AsmInstruction::DWORD)), // first arg
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // second arg
                Assembler::mulss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0()),
            };
        }
    );

    auto TIMES_DOUBLE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::TIMES_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME, Type::DOUBLE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                // convert 1st arg to double
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RCX(Assembler::AsmInstruction::DWORD)), // for optimization
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // second arg
                Assembler::mulsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto DIV_BYTE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::DIV_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{BYTE_PARAM_NAME, Type::BYTE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movd(Assembler::XMM0(), Assembler::RCX(Assembler::AsmInstruction::DWORD)), // first arg
                // convert 2nd arg to float
                Assembler::cbw(),
                Assembler::cwde(),
                Assembler::cvtsi2ss(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::divss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto DIV_INT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::DIV_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{INT_PARAM_NAME, Type::INT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movd(Assembler::XMM0(), Assembler::RCX(Assembler::AsmInstruction::DWORD)), // first arg
                // convert 2nd arg to float
                Assembler::cvtsi2ss(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::divss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto DIV_LONG=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::DIV_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{LONG_PARAM_NAME, Type::LONG}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movd(Assembler::XMM0(), Assembler::RCX(Assembler::AsmInstruction::DWORD)), // first arg
                // convert 2nd arg to float
                Assembler::cvtsi2ss(Assembler::XMM1(), Assembler::RAX()),
                Assembler::divss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto DIV_FLOAT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::DIV_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME, Type::FLOAT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movd(Assembler::XMM0(), Assembler::RCX(Assembler::AsmInstruction::DWORD)), // first arg
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // second arg
                Assembler::divss(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0()),
            };
        }
    );

    auto DIV_DOUBLE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::DIV_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME, Type::DOUBLE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                // convert 1st arg to double
                Assembler::cvtsi2sd(Assembler::XMM0(), Assembler::RCX(Assembler::AsmInstruction::DWORD)), // for optimization
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // second arg
                Assembler::divsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto COMPARE_TO_BYTE=getCompareToFun<PrimitiveType, ByteValue>(
        classScope,
        BYTE_PARAM_NAME,
        Type::BYTE
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

    auto UNARY_MINUS=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::UNARY_MINUS_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>(),
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::_xor(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::imm(L"0x80000000"))
            };
        }
    );

    auto INC=getIncFun<PrimitiveType,FloatValue>(classScope,Type::FLOAT);

    auto DEC=getDecFun<PrimitiveType,FloatValue>(classScope,Type::FLOAT);

    auto TO_BYTE=std::make_shared<BuiltInFunScope>(
        TO_BYTE_NAME,
        Type::BYTE,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movd(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)),
                Assembler::cvtss2si(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0()),
            };
        }
    );

    auto TO_UBYTE=std::make_shared<BuiltInFunScope>(
        TO_UBYTE_NAME,
        Type::UBYTE,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movd(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)),
                Assembler::cvtss2si(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0()),
            };
        }
    );

    auto TO_INT=std::make_shared<BuiltInFunScope>(
        TO_INT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movd(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)),
                Assembler::cvtss2si(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0()),
            };
        }
    );

    auto TO_UINT=std::make_shared<BuiltInFunScope>(
        TO_UINT_NAME,
        Type::UINT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movd(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)),
                Assembler::cvtss2si(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0()),
            };
        }
    );

    auto TO_LONG=std::make_shared<BuiltInFunScope>(
        TO_LONG_NAME,
        Type::LONG,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM0(), Assembler::RAX()),
                Assembler::cvtss2si(Assembler::RAX(), Assembler::XMM0()),
            };
        }
    );

    auto TO_ULONG=std::make_shared<BuiltInFunScope>(
        TO_ULONG_NAME,
        Type::LONG,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM0(), Assembler::RAX()),
                Assembler::cvtss2si(Assembler::RAX(), Assembler::XMM0()),
            };
        }
    );

    auto TO_FLOAT=getToFloatFun<PrimitiveType>(classScope);

    auto TO_DOUBLE=std::make_shared<BuiltInFunScope>(
        TO_DOUBLE_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movd(Assembler::XMM0(), Assembler::RAX(Assembler::AsmInstruction::DWORD)),
                Assembler::cvtss2sd(Assembler::XMM0(), Assembler::XMM0()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto TO_STRING=getToStringFun<PrimitiveType>(classScope);

    auto BIN_REPRESENTATION=std::make_shared<BuiltInFunScope>(
        BIN_REPRESENTATION_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){}
    );

    auto funs={
        PLUS_BYTE,PLUS_INT,PLUS_LONG,PLUS_FLOAT,PLUS_DOUBLE,
        MINUS_BYTE,MINUS_INT,MINUS_LONG,MINUS_FLOAT,MINUS_DOUBLE,
        TIMES_BYTE,TIMES_INT,TIMES_LONG,TIMES_FLOAT,TIMES_DOUBLE,
        DIV_BYTE,DIV_INT,DIV_LONG,DIV_FLOAT,DIV_DOUBLE,
        COMPARE_TO_BYTE,COMPARE_TO_INT,COMPARE_TO_LONG,COMPARE_TO_FLOAT,COMPARE_TO_DOUBLE,
        EQUALS,
        UNARY_PLUS,UNARY_MINUS,
        INC,DEC,
        TO_BYTE,TO_UBYTE,TO_INT,TO_UINT,TO_LONG,TO_ULONG,
        TO_FLOAT,TO_DOUBLE,TO_STRING,
        BIN_REPRESENTATION,
    };

    auto publicFuns=classScope->getPublicFunctions();
    for(auto fun:funs){
        (*publicFuns)[fun->getDecl()->toString()]=fun;
    }

}

void BuiltInFunScope::addBuiltInFunctionsToDoubleClass(){

    auto classScope=std::dynamic_pointer_cast<DoubleClassScope>(Type::DOUBLE->getClassScope());
    
    using PrimitiveType=long double;

    auto PLUS_BYTE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::PLUS_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{BYTE_PARAM_NAME, Type::BYTE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movq(Assembler::XMM0(), Assembler::RCX()), // first arg
                // convert 2nd arg to double
                Assembler::cbw(),
                Assembler::cwde(),
                Assembler::cvtsi2sd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::addsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto PLUS_INT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::PLUS_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{INT_PARAM_NAME, Type::INT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movq(Assembler::XMM0(), Assembler::RCX()), // first arg
                // convert 2nd arg to double
                Assembler::cvtsi2sd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::addsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto PLUS_LONG=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::PLUS_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{LONG_PARAM_NAME, Type::LONG}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movq(Assembler::XMM0(), Assembler::RCX()), // first arg
                // convert 2nd arg to double
                Assembler::cvtsi2sd(Assembler::XMM1(), Assembler::RAX()),
                Assembler::addsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto PLUS_FLOAT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::PLUS_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME, Type::FLOAT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movq(Assembler::XMM0(), Assembler::RCX()), // first arg
                // convert 2nd arg to double
                Assembler::cvtss2sd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::addsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())    
            };
        }
    );

    auto PLUS_DOUBLE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::PLUS_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME, Type::DOUBLE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movq(Assembler::XMM0(), Assembler::RCX()), // first arg
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // second arg
                Assembler::addsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto MINUS_BYTE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::MINUS_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{BYTE_PARAM_NAME, Type::BYTE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movq(Assembler::XMM0(), Assembler::RCX()), // first arg
                // convert 2nd arg to double
                Assembler::cbw(),
                Assembler::cwde(),
                Assembler::cvtsi2sd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::subsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto MINUS_INT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::MINUS_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{INT_PARAM_NAME, Type::INT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movq(Assembler::XMM0(), Assembler::RCX()), // first arg
                // convert 2nd arg to double
                Assembler::cvtsi2sd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::subsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto MINUS_LONG=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::MINUS_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{LONG_PARAM_NAME, Type::LONG}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movq(Assembler::XMM0(), Assembler::RCX()), // first arg
                // convert 2nd arg to double
                Assembler::cvtsi2sd(Assembler::XMM1(), Assembler::RAX()),
                Assembler::subsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto MINUS_FLOAT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::MINUS_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME, Type::FLOAT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movq(Assembler::XMM0(), Assembler::RCX()), // first arg
                // convert 2nd arg to double
                Assembler::cvtss2sd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::subsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())    
            };
        }
    );

    auto MINUS_DOUBLE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::MINUS_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME, Type::DOUBLE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movq(Assembler::XMM0(), Assembler::RCX()), // first arg
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // second arg
                Assembler::subsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto TIMES_BYTE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::TIMES_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{BYTE_PARAM_NAME, Type::BYTE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movq(Assembler::XMM0(), Assembler::RCX()), // first arg
                // convert 2nd arg to double
                Assembler::cbw(),
                Assembler::cwde(),
                Assembler::cvtsi2sd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::mulsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto TIMES_INT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::TIMES_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{INT_PARAM_NAME, Type::INT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movq(Assembler::XMM0(), Assembler::RCX()), // first arg
                // convert 2nd arg to double
                Assembler::cvtsi2sd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::mulsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto TIMES_LONG=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::TIMES_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{LONG_PARAM_NAME, Type::LONG}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movq(Assembler::XMM0(), Assembler::RCX()), // first arg
                // convert 2nd arg to double
                Assembler::cvtsi2sd(Assembler::XMM1(), Assembler::RAX()),
                Assembler::mulsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto TIMES_FLOAT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::TIMES_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME, Type::FLOAT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movq(Assembler::XMM0(), Assembler::RCX()), // first arg
                // convert 2nd arg to double
                Assembler::cvtss2sd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::mulsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())    
            };
        }
    );

    auto TIMES_DOUBLE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::TIMES_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME, Type::DOUBLE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movq(Assembler::XMM0(), Assembler::RCX()), // first arg
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // second arg
                Assembler::mulsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto DIV_BYTE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::DIV_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{BYTE_PARAM_NAME, Type::BYTE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movq(Assembler::XMM0(), Assembler::RCX()), // first arg
                // convert 2nd arg to double
                Assembler::cbw(),
                Assembler::cwde(),
                Assembler::cvtsi2sd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::divsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto DIV_INT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::DIV_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{INT_PARAM_NAME, Type::INT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movq(Assembler::XMM0(), Assembler::RCX()), // first arg
                // convert 2nd arg to double
                Assembler::cvtsi2sd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::divsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto DIV_LONG=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::DIV_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{LONG_PARAM_NAME, Type::LONG}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movq(Assembler::XMM0(), Assembler::RCX()), // first arg
                // convert 2nd arg to double
                Assembler::cvtsi2sd(Assembler::XMM1(), Assembler::RAX()),
                Assembler::divsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto DIV_FLOAT=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::DIV_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{FLOAT_PARAM_NAME, Type::FLOAT}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movq(Assembler::XMM0(), Assembler::RCX()), // first arg
                // convert 2nd arg to double
                Assembler::cvtss2sd(Assembler::XMM1(), Assembler::RAX(Assembler::AsmInstruction::DWORD)), // dword for optimization
                Assembler::divsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())    
            };
        }
    );

    auto DIV_DOUBLE=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::DIV_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>{{DOUBLE_PARAM_NAME, Type::DOUBLE}},
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::pop(Assembler::RCX()),
                Assembler::movq(Assembler::XMM0(), Assembler::RCX()), // first arg
                Assembler::movq(Assembler::XMM1(), Assembler::RAX()), // second arg
                Assembler::divsd(Assembler::XMM0(), Assembler::XMM1()),
                Assembler::movq(Assembler::RAX(), Assembler::XMM0())
            };
        }
    );

    auto COMPARE_TO_BYTE=getCompareToFun<PrimitiveType, ByteValue>(
        classScope,
        BYTE_PARAM_NAME,
        Type::BYTE
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

    auto UNARY_MINUS=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::UNARY_MINUS_NAME,
        Type::DOUBLE,
        std::vector<std::pair<std::wstring, SharedType>>(),
        [](Interpreter* interpreter){},
        true,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::mov(Assembler::RCX(), Assembler::imm(L"0x8000000000000000")),
                Assembler::_xor(Assembler::RAX(), Assembler::RCX())
            };
        }
    );

    auto INC=getIncFun<PrimitiveType,DoubleValue>(classScope,Type::DOUBLE);

    auto DEC=getDecFun<PrimitiveType,DoubleValue>(classScope,Type::DOUBLE);

    auto TO_BYTE=std::make_shared<BuiltInFunScope>(
        TO_BYTE_NAME,
        Type::BYTE,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM0(), Assembler::RAX()),
                Assembler::cvtsd2si(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0()),
            };
        }
    );

    auto TO_UBYTE=std::make_shared<BuiltInFunScope>(
        TO_UBYTE_NAME,
        Type::UBYTE,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM0(), Assembler::RAX()),
                Assembler::cvtsd2si(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0()),
            };
        }
    );

    auto TO_INT=std::make_shared<BuiltInFunScope>(
        TO_INT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM0(), Assembler::RAX()),
                Assembler::cvtsd2si(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0()),
            };
        }
    );

    auto TO_UINT=std::make_shared<BuiltInFunScope>(
        TO_UINT_NAME,
        Type::UINT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM0(), Assembler::RAX()),
                Assembler::cvtsd2si(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0()),
            };
        }
    );

    auto TO_LONG=std::make_shared<BuiltInFunScope>(
        TO_LONG_NAME,
        Type::LONG,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM0(), Assembler::RAX()),
                Assembler::cvtsd2si(Assembler::RAX(), Assembler::XMM0()),
            };
        }
    );

    auto TO_ULONG=std::make_shared<BuiltInFunScope>(
        TO_ULONG_NAME,
        Type::ULONG,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM0(), Assembler::RAX()),
                Assembler::cvtsd2si(Assembler::RAX(), Assembler::XMM0()),
            };
        }
    );

    auto TO_FLOAT=std::make_shared<BuiltInFunScope>(
        TO_FLOAT_NAME,
        Type::FLOAT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::movq(Assembler::XMM0(), Assembler::RAX()),
                Assembler::cvtsd2ss(Assembler::XMM0(), Assembler::XMM0()),
                Assembler::movd(Assembler::RAX(Assembler::AsmInstruction::DWORD), Assembler::XMM0())
            };
        }
    );

    auto TO_DOUBLE=getToDoubleFun<PrimitiveType>(classScope);

    auto TO_STRING=getToStringFun<PrimitiveType>(classScope);

    auto BIN_REPRESENTATION=std::make_shared<BuiltInFunScope>(
        BIN_REPRESENTATION_NAME,
        Type::LONG,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){}
    );

    auto funs={
        PLUS_BYTE,PLUS_INT,PLUS_LONG,PLUS_FLOAT,PLUS_DOUBLE,
        MINUS_BYTE,MINUS_INT,MINUS_LONG,MINUS_FLOAT,MINUS_DOUBLE,
        TIMES_BYTE,TIMES_INT,TIMES_LONG,TIMES_FLOAT,TIMES_DOUBLE,
        DIV_BYTE,DIV_INT,DIV_LONG,DIV_FLOAT,DIV_DOUBLE,
        COMPARE_TO_BYTE,COMPARE_TO_INT,COMPARE_TO_LONG,COMPARE_TO_FLOAT,COMPARE_TO_DOUBLE,
        EQUALS,
        UNARY_PLUS,UNARY_MINUS,
        INC,DEC,
        TO_BYTE,TO_UBYTE,TO_INT,TO_UINT,TO_LONG,TO_ULONG,
        TO_FLOAT,TO_DOUBLE,TO_STRING,
        BIN_REPRESENTATION,
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

    auto TO_INT=std::make_shared<BuiltInFunScope>(
        TO_INT_NAME,
        Type::INT,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            return std::vector{
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0x1"))
            };
        }
    );

    auto TO_STRING=std::make_shared<BuiltInFunScope>(
        TO_STRING_NAME,
        Type::STRING,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){
            interpreter->AX=std::make_shared<StringValue>(interpreter->AX->toString());
        }
    );

    auto funs={
        NOT,
        EQUALS,
        TO_INT,
        TO_STRING,
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
        true,
        [=](Compiler* compiler){
            auto memcpyLabel=compiler->addAinMemcpyAsm();
            auto allocLabel=compiler->addAinAllocAsm();

            return std::vector{
                Assembler::pop(Assembler::R10()), // The call address
                Assembler::pop(Assembler::RDI()), // The address of first string (from the stack)
                Assembler::mov(Assembler::R8(), Assembler::addressMov(Assembler::RDI())), // The size of first string
                Assembler::mov(Assembler::R9(), Assembler::addressMov(Assembler::RAX())), // The size of second string

                 // The total size after concatenation, add 8 bytes for the size property
                Assembler::lea(Assembler::RDX(), Assembler::addressLea(Assembler::R8().value+L"+8+"+Assembler::R9().value)),
                Assembler::lea(Assembler::RDI(), Assembler::addressLea(Assembler::RDI().value+L"+8")), // the address of first char in first string
                Assembler::lea(Assembler::RAX(), Assembler::addressLea(Assembler::RAX().value+L"+8")), // the address of first char in second string
                
                Assembler::push(Assembler::R10()), // The call address
                Assembler::push(Assembler::imm(L"0")), // preserve space for the new allocated string (for final return)

                Assembler::push(Assembler::RDI()), // preserve first string first char address (from arg for memcpy)
                Assembler::push(Assembler::imm(L"0")), // preserve space for the new allocated string (to arg for memcpy)
                Assembler::push(Assembler::R8()), // preserve the size of first string (size arg for memcpy)

                Assembler::push(Assembler::RAX()), // preserve the address of second string (from arg for memcpy)
                Assembler::push(Assembler::imm(L"0")), // preserve space for the new allocated string (to arg for memcpy)
                Assembler::push(Assembler::R9()), // preserve the size of second string (size arg for memcpy)

                Assembler::push(Assembler::RDX(), L"مُعامل الحجم_بالبايت"),
                Assembler::call(Assembler::label(allocLabel), L"استدعاء دالة احجز(كبير)"),
                Assembler::pop(Assembler::RDX()), // The total size + 8 bytes

                Assembler::lea(Assembler::RDX(), Assembler::addressLea(Assembler::RDX().value+L"-8")), // restore the total size after concatenation
                Assembler::mov(Assembler::addressMov(Assembler::RAX()), Assembler::RDX()), // Write the total size after concatenation

                Assembler::mov(Assembler::addressMov(Assembler::RSP(),48), Assembler::RAX()), // write new string address for final return 
                Assembler::lea(Assembler::RAX(), Assembler::addressLea(Assembler::RAX().value+L"+8")), // The pointer of first char in new allocated string
                Assembler::mov(Assembler::addressMov(Assembler::RSP(),32), Assembler::RAX()), // write 'to' arg for first memcpy
                Assembler::add(Assembler::RAX(), Assembler::addressMov(Assembler::RSP(), 24)), // The pointer of (the prev pointer + first string size)
                Assembler::mov(Assembler::addressMov(Assembler::RSP(),8), Assembler::RAX()), // write 'to' arg for second memcpy

                // second memcpy
                Assembler::call(Assembler::label(memcpyLabel), L"استدعاء دالة انسخ(كبير، كبير، كبير)"),
                Assembler::removeReservedSpaceFromStack(24),
                // first memcpy
                Assembler::call(Assembler::label(memcpyLabel), L"استدعاء دالة انسخ(كبير، كبير، كبير)"),
                Assembler::removeReservedSpaceFromStack(24),

                Assembler::pop(Assembler::RAX()), // final return
                Assembler::ret()
            };
        }
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
        true,
        [=](Compiler* compiler){
            auto memcpyLabel=compiler->addAinMemcpyAsm();
            auto allocLabel=compiler->addAinAllocAsm();
            std::wstring cntCharDoneLabel=L"cntCharDone";

            return std::vector{
                Assembler::pop(Assembler::R10()), // The call address
                Assembler::pop(Assembler::RDI()), // The address of the string (from the stack)
                
                Assembler::mov(Assembler::RDX(), Assembler::addressMov(Assembler::RDI())), // The size of string
                Assembler::mov(Assembler::RSI(), Assembler::RAX()), // The char
                Assembler::lea(Assembler::RDI(), Assembler::addressLea(Assembler::RDI().value+L"+8")), // The pointer of first char in the string
                
                Assembler::_not(Assembler::RAX()),
                Assembler::_and(Assembler::RAX(), Assembler::imm(L"0xFF")),
                Assembler::bsr(Assembler::RCX(), Assembler::RAX()),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"7")),
                Assembler::sub(Assembler::RAX(), Assembler::RCX()),
                Assembler::test(Assembler::RAX(), Assembler::RAX()),
                Assembler::jnz(Assembler::label(L"."+cntCharDoneLabel)),
                Assembler::mov(Assembler::RAX(), Assembler::imm(L"1")),
                
                Assembler::localLabel(cntCharDoneLabel),
                Assembler::push(Assembler::R10()), // call address
                Assembler::push(Assembler::imm(L"0")), // preserve space for new string for final return

                Assembler::push(Assembler::RSI()), // char
                Assembler::push(Assembler::RAX()), // char size

                Assembler::push(Assembler::RDI()), // string first char address (from arg)
                Assembler::push(Assembler::imm(L"0")), // preserve space for (to arg for memcpy)
                Assembler::push(Assembler::RDX()), // string size

                // new size of string + 8 bytes for size field
                Assembler::lea(Assembler::RAX(), Assembler::addressLea(Assembler::RDX().value+L"+8+"+Assembler::RAX().value)),
                Assembler::push(Assembler::RAX(), L"مُعامل الحجم_بالبايت"), // new size to allocate
                Assembler::call(Assembler::label(allocLabel), L"استدعاء دالة احجز(كبير)"),
                Assembler::pop(Assembler::RDX()), // The total size + 8 bytes

                Assembler::lea(Assembler::RDX(), Assembler::addressLea(Assembler::RDX().value+L"-8")), // restore the total size after concatenation
                Assembler::mov(Assembler::addressMov(Assembler::RAX()), Assembler::RDX()), // Write the total size after concatenation

                Assembler::mov(Assembler::addressMov(Assembler::RSP(),40), Assembler::RAX()), // write new string address for final return 
                Assembler::lea(Assembler::RAX(), Assembler::addressLea(Assembler::RAX().value+L"+8")), // The pointer of first char in new allocated string
                Assembler::mov(Assembler::addressMov(Assembler::RSP(),8), Assembler::RAX()), // write 'to' arg for memcpy

                // memcpy first string
                Assembler::call(Assembler::label(memcpyLabel), L"استدعاء دالة انسخ(كبير، كبير، كبير)"),
                Assembler::pop(Assembler::RDX()), // size of string
                Assembler::pop(Assembler::RDI()), // to arg
                Assembler::pop(Assembler::RSI()), // from arg

                Assembler::pop(Assembler::RCX()), // char size
                Assembler::pop(Assembler::RAX()), // char

                // The address to write the char in it
                Assembler::lea(Assembler::RDI(), Assembler::addressLea(Assembler::RDI().value+L"+"+Assembler::RDX().value)),
                // The address of char address + char size to preserve the content of it, as when writing a 3-byte char, this maybe destroy what after the char
                Assembler::lea(Assembler::RSI(), Assembler::addressLea(Assembler::RDI().value+L"+"+Assembler::RCX().value)),
                // Preserve what after the char address
                Assembler::mov(Assembler::R8(), Assembler::addressMov(Assembler::RSI())),
                // Write char
                Assembler::mov(Assembler::addressMov(Assembler::RDI()), Assembler::RAX()),
                // Write what after char (restore it)
                Assembler::mov(Assembler::addressMov(Assembler::RSI()), Assembler::R8()),

                Assembler::pop(Assembler::RAX()), // final return the new string
                Assembler::ret()
            };
        }
    );

    auto GET=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::GET_NAME,
        Type::UBYTE,
        std::vector<std::pair<std::wstring, SharedType>>{{INDEX_PARAM_NAME,Type::ULONG}},
        [](Interpreter* interpreter){},
        true
        // The compilation is same as array
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

    auto TO_BYTE=std::make_shared<BuiltInFunScope>(
        TO_BYTE_NAME,
        Type::BYTE,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){
            auto val=interpreter->AX->toString();
            try{
                auto value=std::stoi(val);
                if (value>std::numeric_limits<char>().max()||value<std::numeric_limits<char>().min())
                    throw NumberFormatException(val);
                
                interpreter->AX=std::make_shared<ByteValue>(value);
            }catch(std::exception e){
                throw NumberFormatException(val);
            }
        }
    );

    auto TO_UBYTE=std::make_shared<BuiltInFunScope>(
        TO_UBYTE_NAME,
        Type::UBYTE,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){
            auto val=interpreter->AX->toString();
            try{
                auto value=std::stoull(val);

                if (value>std::numeric_limits<unsigned char>().max())
                    throw NumberFormatException(val);
                
                interpreter->AX=std::make_shared<ByteValue>(value);
            }catch(std::exception e){
                throw NumberFormatException(val);
            }
        }
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
        GET,
        EQUALS,
        TO_BYTE,TO_UBYTE,
        TO_INT,TO_UINT,
        TO_LONG,TO_ULONG,
        TO_FLOAT,TO_DOUBLE,
        TO_STRING
    };

    auto publicFuns=classScope->getPublicFunctions();

    for(auto fun:funs){
        (*publicFuns)[fun->getDecl()->toString()]=fun;
    }

    auto CONSTRUCTOR_FROM_BYTE_ARRAY=std::make_shared<BuiltInFunScope>(
        KeywordToken::NEW.getVal(),
        Type::STRING,
        std::vector<std::pair<std::wstring, SharedType>>{
            {L"مصفوفة_م1",std::make_shared<Type>(Type::Array(Type::UBYTE))}
        },
        [](Interpreter* interpreter){},
        false,
        [=](Compiler* compiler){
            auto memcpyLabel=compiler->addAinMemcpyAsm();
            auto allocLabel=compiler->addAinAllocAsm();
            return std::vector{
                Assembler::mov(Assembler::RDX(), Assembler::addressMov(Assembler::RAX())), // The size of array
                Assembler::push(Assembler::imm(L"0")), // preserve space for new string for final return
                Assembler::push(Assembler::RAX()), // preserve 'from' arg for memcpy
                Assembler::push(Assembler::imm(L"0")), // preserve space for 'to' arg for memcpy
                Assembler::lea(Assembler::RDX(), Assembler::addressLea(Assembler::RDX().value+L"+8")), // add 8 bytes for size field
                Assembler::push(Assembler::RDX(), L"مُعامل الحجم_بالبايت"), // The size to allocate, also arg for memcpy
                Assembler::call(Assembler::label(allocLabel), L"استدعاء دالة احجز(كبير)"),
                Assembler::mov(Assembler::addressMov(Assembler::RSP(),8), Assembler::RAX()), // 'to' arg for memcpy
                Assembler::mov(Assembler::addressMov(Assembler::RSP(),24), Assembler::RAX()), // final return string
                // memcpy
                Assembler::call(Assembler::label(memcpyLabel), L"استدعاء دالة انسخ(كبير، كبير، كبير)"),
                Assembler::removeReservedSpaceFromStack(24),
                Assembler::pop(Assembler::RAX()), // final return
                Assembler::ret()
            };
        }
    );

    auto decl=CONSTRUCTOR_FROM_BYTE_ARRAY->getDecl()->toString();

    if(!classScope->findPublicConstructor(decl))
        (*classScope->getPublicConstructors())[decl]=CONSTRUCTOR_FROM_BYTE_ARRAY;

}

void BuiltInFunScope::addBuiltInFunctionsToVoidClass(){

    auto TO_STRING=std::make_shared<BuiltInFunScope>(
        TO_STRING_NAME,
        Type::STRING,
        std::vector<std::pair<std::wstring, SharedType>>{},
        [](Interpreter* interpreter){
            auto val=interpreter->AX;
            interpreter->AX=std::make_shared<StringValue>(val->toString());
        }
    );

    auto publicFuns=Type::VOID->getClassScope()->getPublicFunctions();
   
    (*publicFuns)[TO_STRING->getDecl()->toString()]=TO_STRING;

}

void BuiltInFunScope::addBuiltInFunctionsToArrayClass(){
    auto classScope=Type::ARRAY_CLASS;

    auto genericType=std::make_shared<Type>(std::make_shared<std::wstring>(L""));

    auto GET=std::make_shared<BuiltInFunScope>(
        OperatorFunctions::GET_NAME,
        genericType,
        std::vector<std::pair<std::wstring, SharedType>>{{INDEX_PARAM_NAME,Type::ULONG}},
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
        Type::VOID,
        std::vector<std::pair<std::wstring, SharedType>>{
            {INDEX_PARAM_NAME,Type::ULONG},
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
            interpreter->AX=std::make_shared<VoidValue>();
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