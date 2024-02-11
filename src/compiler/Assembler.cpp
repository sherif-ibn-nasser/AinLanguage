#include "Assembler.hpp"
#include <algorithm>
#include <iterator>
#include <string>

namespace Assembler {

    std::wstring AsmInstruction::getAsmText(bool showComment){
        std::wstring text;
        switch (this->type) {
            case LOCAL_LABEL:
                return operands[0].value;
            case NOP:
                text=L"nop";break;
            case PUSH:
                text=L"push";break;
            case POP
                :text=L"pop";break;
            case MOV:
                text=L"mov";break;
            case LEA:
                text=L"lea";break;
            case ADD:
                text=L"add";break;
            case SUB:
                text=L"sub";break;
            case XOR:
                text=L"xor";break;
            case CALL:
                text=L"call";break;
            case RET:
                text=L"ret";break;
            case SYSCALL:
                text=L"syscall";break;
            case CMP:
                text=L"cmp";break;
            case TEST:
                text=L"test";break;
            case JMP:
                text=L"jmp";break;
            case JZ:
                text=L"jz";break;
            case JNZ:
                text=L"jnz";break;
            }
        auto i=0;
        for (auto &operand : this->operands) {
            text+=((i==0)?L" ":L", ")+operand.value;
            i++;
        }
        if(!this->comment.empty()&&showComment)
            text+=L"\t; "+comment;
        return text;
    }

    void AsmLabel::operator+=(AsmInstruction instruction){
        if(instruction.type==AsmInstruction::NOP)
            return;
        this->instructions.push_back(instruction);
    }

    void AsmLabel::operator+=(std::vector<AsmInstruction> instructions){
        std::copy_if(
            instructions.begin(),
            instructions.end(),
            std::back_inserter(this->instructions),
            [](const auto &val){return val.type!=AsmInstruction::NOP;}
        );
    }

    std::wstring AsmLabel::getAsmText(){
        auto text=this->label+L":";
        if(!this->comment.empty())
            text+=L"\t; "+comment;
        for (auto& instruction : this->instructions) {
            text+=L"\n";
            if(instruction.type==AsmInstruction::LOCAL_LABEL)
                text+=L"\n";
            else
                text+=L"\t";
            text+=instruction.getAsmText();
        }
        return text;
    }

    AsmInstruction localLabel(std::wstring label, std::wstring comment){
        return AsmInstruction{
            .type=AsmInstruction::LOCAL_LABEL,
            .operands={
                Assembler::label(L"."+label+L":")
            },
            .comment=comment,
        };
    }

    AsmInstruction nop(std::wstring comment){
        return AsmInstruction{.type=AsmInstruction::NOP, .comment=comment};
    }

    AsmInstruction push(AsmOperand op, std::wstring comment){
        return AsmInstruction{
            .type=AsmInstruction::PUSH,
            .operands={op},
            .comment=comment
        };
    }

    AsmInstruction pop(AsmOperand op, std::wstring comment){
        return AsmInstruction{
            .type=AsmInstruction::POP,
            .operands={op},
            .comment=comment
        };
    }

    AsmInstruction mov(AsmOperand d, AsmOperand s, AsmInstruction::InstructionSize size, std::wstring comment){
        return AsmInstruction{
            .type=AsmInstruction::MOV,
            .operands={d, s},
            .comment=comment
        };
    }

    AsmInstruction lea(AsmOperand d, AsmOperand s, AsmInstruction::InstructionSize size, std::wstring comment){
        return AsmInstruction{
            .type=AsmInstruction::LEA,
            .operands={d, s},
            .comment=comment
        };
    }

    AsmInstruction add(AsmOperand d, AsmOperand s, std::wstring comment){
        return AsmInstruction{
            .type=AsmInstruction::ADD,
            .operands={d, s},
            .comment=comment
        };
    }

    AsmInstruction sub(AsmOperand d, AsmOperand s, std::wstring comment){
        return AsmInstruction{
            .type=AsmInstruction::SUB,
            .operands={d, s},
            .comment=comment
        };
    }

    AsmInstruction _xor(AsmOperand d, AsmOperand s, std::wstring comment){
        return AsmInstruction{
            .type=AsmInstruction::XOR,
            .operands={d, s},
            .comment=comment
        };
    }

    AsmInstruction zero(AsmOperand d, std::wstring comment){
        return _xor(d, d, comment);
    }

    AsmInstruction call(AsmOperand label, std::wstring comment){
        return AsmInstruction{
            .type=AsmInstruction::CALL,
            .operands={label},
            .comment=comment
        };
    }

    AsmInstruction ret(std::wstring comment){
        return AsmInstruction{
            .type=AsmInstruction::RET,
            .comment=comment
        };
    }

    AsmInstruction syscall(std::wstring comment){
        return AsmInstruction{
            .type=AsmInstruction::SYSCALL,
            .comment=comment
        };
    }

    AsmInstruction cmp(AsmOperand s1, AsmOperand s2, std::wstring comment){
        return AsmInstruction{
            .type=AsmInstruction::CMP,
            .operands={s1, s2},
            .comment=comment
        };
    }

    AsmInstruction test(AsmOperand s1, AsmOperand s2, std::wstring comment){
        return AsmInstruction{
            .type=AsmInstruction::TEST,
            .operands={s1, s2},
            .comment=comment
        };
    }

    AsmInstruction jmp(AsmOperand label, std::wstring comment){
        return AsmInstruction{
            .type=AsmInstruction::JMP,
            .operands={label},
            .comment=comment
        };
    }

    AsmInstruction jz(AsmOperand label, std::wstring comment){
        return AsmInstruction{
            .type=AsmInstruction::JZ,
            .operands={label},
            .comment=comment
        };
    }

    AsmInstruction jnz(AsmOperand label, std::wstring comment){
        return AsmInstruction{
            .type=AsmInstruction::JNZ,
            .operands={label},
            .comment=comment
        };
    }

    AsmInstruction reserveSpaceOnStack(int size, std::wstring comment){
        if(size==0)
            return nop(comment);
        return sub(RSP(),imm(std::to_wstring(size)),comment);
    }

    AsmInstruction removeReservedSpaceFromStack(int size, std::wstring comment){
        if(size==0)
            return nop(comment);
        return add(RSP(),imm(std::to_wstring(size)),comment);
    }

    std::vector<AsmInstruction> exit(int errorCode, std::wstring comment){
        return {
            mov(RAX(),imm(L"60"),AsmInstruction::IMPLICIT,comment),
            (errorCode==0)
                ?zero(RDI())
                :mov(RDI(), imm(std::to_wstring(errorCode))),
            syscall()
        };
    }

    AsmOperand label(std::wstring label){
        return AsmOperand{.type=AsmOperand::LABEL, .value=label};
    }

    AsmOperand addressMov(AsmOperand op, int offset){
        auto value=L"["+op.value;

        if(offset>0)
            value+=L"+"+std::to_wstring(offset);
        if(offset<0)
            value+=std::to_wstring(offset);

        value+=L"]";

        return AsmOperand{.type=AsmOperand::ADDRESSING, .value=value};
    }

    AsmOperand addressLea(std::wstring address){
        auto value=L"["+address+L"]";
        return AsmOperand{.type=AsmOperand::ADDRESSING, .value=value};
    }

    AsmOperand imm(std::wstring value){
        return AsmOperand{.type=AsmOperand::IMM, .value=value};
    }

    AsmOperand RAX(int size){
        auto value=L"";
        switch (size) {
            case AsmInstruction::BYTE:
                value=L"AL"; break;
            case AsmInstruction::WORD:
                value=L"AX"; break;
            case AsmInstruction::DWORD:
                value=L"EAX"; break;
            default:
                value=L"RAX"; break;
        }
        return AsmOperand{.type=AsmOperand::REG, .value=value};
    }

    AsmOperand RBX(int size){
        auto value=L"";
        switch (size) {
            case AsmInstruction::BYTE:
                value=L"BL"; break;
            case AsmInstruction::WORD:
                value=L"BX"; break;
            case AsmInstruction::DWORD:
                value=L"EBX"; break;
            default:
                value=L"RBX"; break;
        }
        return AsmOperand{.type=AsmOperand::REG, .value=value};
    }

    AsmOperand RCX(int size){
        auto value=L"";
        switch (size) {
            case AsmInstruction::BYTE:
                value=L"CL"; break;
            case AsmInstruction::WORD:
                value=L"CX"; break;
            case AsmInstruction::DWORD:
                value=L"ECX"; break;
            default:
                value=L"RCX"; break;
        }
        return AsmOperand{.type=AsmOperand::REG, .value=value};
    }

    AsmOperand RDX(int size){
        auto value=L"";
        switch (size) {
            case AsmInstruction::BYTE:
                value=L"DL"; break;
            case AsmInstruction::WORD:
                value=L"DX"; break;
            case AsmInstruction::DWORD:
                value=L"EDX"; break;
            default:
                value=L"RDX"; break;
        }
        return AsmOperand{.type=AsmOperand::REG, .value=value};
    }

    AsmOperand RDI(int size){
        auto value=L"";
        switch (size) {
            case AsmInstruction::BYTE:
                value=L"DIL"; break;
            case AsmInstruction::WORD:
                value=L"DI"; break;
            case AsmInstruction::DWORD:
                value=L"EDI"; break;
            default:
                value=L"RDI"; break;
        }
        return AsmOperand{.type=AsmOperand::REG, .value=value};
    }

    AsmOperand RSI(int size){
        auto value=L"";
        switch (size) {
            case AsmInstruction::BYTE:
                value=L"SIL"; break;
            case AsmInstruction::WORD:
                value=L"SI"; break;
            case AsmInstruction::DWORD:
                value=L"ESI"; break;
            default:
                value=L"RSI"; break;
        }
        return AsmOperand{.type=AsmOperand::REG, .value=value};
    }

    AsmOperand RSP(int size){
        auto value=L"";
        switch (size) {
            case AsmInstruction::BYTE:
                value=L"SPL"; break;
            case AsmInstruction::WORD:
                value=L"SP"; break;
            case AsmInstruction::DWORD:
                value=L"ESP"; break;
            default:
                value=L"RSP"; break;
        }
        return AsmOperand{.type=AsmOperand::REG, .value=value};
    }

    AsmOperand RBP(int size){
        auto value=L"";
        switch (size) {
            case AsmInstruction::BYTE:
                value=L"BPL"; break;
            case AsmInstruction::WORD:
                value=L"BP"; break;
            case AsmInstruction::DWORD:
                value=L"EBP"; break;
            default:
                value=L"RBP"; break;
        }
        return AsmOperand{.type=AsmOperand::REG, .value=value};
    }

    AsmOperand R8(int size){
        auto value=L"";
        switch (size) {
            case AsmInstruction::BYTE:
                value=L"R8B"; break;
            case AsmInstruction::WORD:
                value=L"R8W"; break;
            case AsmInstruction::DWORD:
                value=L"R8D"; break;
            default:
                value=L"R8"; break;
        }
        return AsmOperand{.type=AsmOperand::REG, .value=value};
    }

    AsmOperand R9(int size){
        auto value=L"";
        switch (size) {
            case AsmInstruction::BYTE:
                value=L"R9B"; break;
            case AsmInstruction::WORD:
                value=L"R9W"; break;
            case AsmInstruction::DWORD:
                value=L"R9D"; break;
            default:
                value=L"R9"; break;
        }
        return AsmOperand{.type=AsmOperand::REG, .value=value};
    }

    AsmOperand R10(int size){
        auto value=L"";
        switch (size) {
            case AsmInstruction::BYTE:
                value=L"R10B"; break;
            case AsmInstruction::WORD:
                value=L"R10W"; break;
            case AsmInstruction::DWORD:
                value=L"R10D"; break;
            default:
                value=L"R10"; break;
        }
        return AsmOperand{.type=AsmOperand::REG, .value=value};
    }

    AsmOperand R12(int size){
        auto value=L"";
        switch (size) {
            case AsmInstruction::BYTE:
                value=L"R12B"; break;
            case AsmInstruction::WORD:
                value=L"R12W"; break;
            case AsmInstruction::DWORD:
                value=L"R12D"; break;
            default:
                value=L"R12"; break;
        }
        return AsmOperand{.type=AsmOperand::REG, .value=value};
    }

    AsmOperand brk_end(){
        return AsmOperand{.type=AsmOperand::REG, .value=L"brk_end"}; // simulate that it as a register
    }

}