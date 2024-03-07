#pragma once

#include "SharedPtrTypes.hpp"
#include <string>
#include <vector>

namespace Assembler {

    struct AsmOperand{
        enum OperandType{
            IMM,
            REG,
            ADDRESSING,
            LABEL
        };

        OperandType type;
        std::wstring value;

    };

    struct AsmInstruction{
        // TODO: Add more instructions
        enum InstructionType{
            LOCAL_LABEL,
            NOP,
            PUSH,
            POP,
            MOV,
            LEA,
            ADD,
            SUB,
            XOR,
            OR,
            AND,
            SHR,
            SHL,
            CALL,
            RET,
            SYSCALL,
            CMP,
            TEST,
            JMP,
            JZ,
            JNZ,
            JS,
            JNS,
            JG,
            JGE,
            JL,
            JLE,
            JA,
            JAE,
            JB,
            JBE,
            INC,
            DEC,
            NEG,
            NOT,
            MUL,
            IMUL,
            DIV,
            IDIV,
            SETZ,
            SETNZ,
            SETS,
            SETNS,
            SETG,
            SETGE,
            SETL,
            SETLE,
            SETA,
            SETAE,
            SETB,
            SETBE,
            CBW,
            CWDE,
            CDQE,
            CWD,
            CDQ,
            CMOVZ,
            CMOVS,
            CMOVG,
        };

        enum InstructionSize{
            IMPLICIT=0, // means the size is determined in registers
            BYTE=1,
            WORD=2,
            DWORD=4,
            QWORD=8,
        };

        InstructionType type;
        InstructionSize size=IMPLICIT;
        std::vector<AsmOperand> operands;
        std::wstring comment=L"";
        std::wstring getAsmText(bool showComment=true);

    };

    struct AsmLabel{
        std::wstring label;
        std::vector<AsmInstruction> instructions;
        std::wstring comment=L"";
        void operator+=(AsmInstruction instruction);
        void operator+=(std::vector<AsmInstruction> instructions);
        std::wstring getAsmText();
    };

    AsmInstruction localLabel(std::wstring label, std::wstring comment=L"");
    AsmInstruction nop(std::wstring comment=L"");
    AsmInstruction push(AsmOperand op, std::wstring comment=L"");
    AsmInstruction pop(AsmOperand op, std::wstring comment=L"");
    AsmInstruction mov(AsmOperand d, AsmOperand s, AsmInstruction::InstructionSize size=AsmInstruction::IMPLICIT, std::wstring comment=L"");
    AsmInstruction lea(AsmOperand d, AsmOperand s, AsmInstruction::InstructionSize size=AsmInstruction::IMPLICIT, std::wstring comment=L"");
    AsmInstruction add(AsmOperand d, AsmOperand s, std::wstring comment=L"");
    AsmInstruction sub(AsmOperand d, AsmOperand s, std::wstring comment=L"");
    AsmInstruction _xor(AsmOperand d, AsmOperand s, std::wstring comment=L"");
    AsmInstruction _or(AsmOperand d, AsmOperand s, std::wstring comment=L"");
    AsmInstruction _and(AsmOperand d, AsmOperand s, std::wstring comment=L"");
    AsmInstruction shr(AsmOperand d, AsmOperand k, std::wstring comment=L"");
    AsmInstruction shl(AsmOperand d, AsmOperand k, std::wstring comment=L"");
    AsmInstruction zero(AsmOperand d, std::wstring comment=L"");
    AsmInstruction call(AsmOperand label, std::wstring comment=L"");
    AsmInstruction ret(std::wstring comment=L"");
    AsmInstruction syscall(std::wstring comment=L"");
    AsmInstruction cmp(AsmOperand s1, AsmOperand s2, std::wstring comment=L"");
    AsmInstruction test(AsmOperand s1, AsmOperand s2, std::wstring comment=L"");
    AsmInstruction jmp(AsmOperand label, std::wstring comment=L"");
    AsmInstruction jz(AsmOperand label, std::wstring comment=L"");
    AsmInstruction jnz(AsmOperand label, std::wstring comment=L"");
    AsmInstruction inc(AsmOperand d, AsmInstruction::InstructionSize size=AsmInstruction::IMPLICIT, std::wstring comment=L"");
    AsmInstruction dec(AsmOperand d, AsmInstruction::InstructionSize size=AsmInstruction::IMPLICIT, std::wstring comment=L"");
    AsmInstruction neg(AsmOperand d, AsmInstruction::InstructionSize size=AsmInstruction::IMPLICIT, std::wstring comment=L"");
    AsmInstruction _not(AsmOperand d, AsmInstruction::InstructionSize size=AsmInstruction::IMPLICIT, std::wstring comment=L"");
    AsmInstruction mul(AsmOperand s, AsmInstruction::InstructionSize size=AsmInstruction::IMPLICIT, std::wstring comment=L"");
    AsmInstruction imul(AsmOperand s, AsmInstruction::InstructionSize size=AsmInstruction::IMPLICIT, std::wstring comment=L"");
    AsmInstruction div(AsmOperand s, AsmInstruction::InstructionSize size=AsmInstruction::IMPLICIT, std::wstring comment=L"");
    AsmInstruction idiv(AsmOperand s, AsmInstruction::InstructionSize size=AsmInstruction::IMPLICIT, std::wstring comment=L"");
    AsmInstruction setz(AsmOperand s, std::wstring comment=L"");
    AsmInstruction setnz(AsmOperand s, std::wstring comment=L"");
    AsmInstruction sets(AsmOperand s, std::wstring comment=L"");
    AsmInstruction setns(AsmOperand s, std::wstring comment=L"");
    AsmInstruction setg(AsmOperand s, std::wstring comment=L"");
    AsmInstruction setge(AsmOperand s, std::wstring comment=L"");
    AsmInstruction setl(AsmOperand s, std::wstring comment=L"");
    AsmInstruction setle(AsmOperand s, std::wstring comment=L"");
    AsmInstruction seta(AsmOperand s, std::wstring comment=L"");
    AsmInstruction setae(AsmOperand s, std::wstring comment=L"");
    AsmInstruction setb(AsmOperand s, std::wstring comment=L"");
    AsmInstruction setbe(AsmOperand s, std::wstring comment=L"");
    AsmInstruction cbw(std::wstring comment=L"");
    AsmInstruction cwde(std::wstring comment=L"");
    AsmInstruction cdqe(std::wstring comment=L"");
    AsmInstruction cwd(std::wstring comment=L"");
    AsmInstruction cdq(std::wstring comment=L"");
    AsmInstruction cmovz(AsmOperand d, AsmOperand s, AsmInstruction::InstructionSize size=AsmInstruction::IMPLICIT, std::wstring comment=L"");
    AsmInstruction cmovs(AsmOperand d, AsmOperand s, AsmInstruction::InstructionSize size=AsmInstruction::IMPLICIT, std::wstring comment=L"");
    AsmInstruction cmovg(AsmOperand d, AsmOperand s, AsmInstruction::InstructionSize size=AsmInstruction::IMPLICIT, std::wstring comment=L"");

    AsmInstruction reserveSpaceOnStack(int size, std::wstring comment=L"");
    AsmInstruction removeReservedSpaceFromStack(int size, std::wstring comment=L"");
    std::vector<AsmInstruction> exit(int errorCode, std::wstring comment=L"");

    AsmOperand label(std::wstring label);
    AsmOperand addressMov(AsmOperand op, int offset=0);
    AsmOperand addressLea(std::wstring address);
    AsmOperand imm(std::wstring value);

    AsmOperand RAX(int size=AsmInstruction::QWORD);
    AsmOperand RBX(int size=AsmInstruction::QWORD);
    AsmOperand RCX(int size=AsmInstruction::QWORD);
    AsmOperand RDX(int size=AsmInstruction::QWORD);
    AsmOperand RDI(int size=AsmInstruction::QWORD);
    AsmOperand RSI(int size=AsmInstruction::QWORD);
    AsmOperand RSP(int size=AsmInstruction::QWORD);
    AsmOperand RBP(int size=AsmInstruction::QWORD);
    AsmOperand R8(int size=AsmInstruction::QWORD);
    AsmOperand R9(int size=AsmInstruction::QWORD);
    AsmOperand R10(int size=AsmInstruction::QWORD);
    AsmOperand brk_end();

    AsmInstruction::InstructionSize size(int size);
    AsmInstruction::InstructionSize getSizeOfReg(AsmOperand reg);

}