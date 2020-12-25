#include "disassembler.h"
#include <stdio.h>
#include <string.h>


Disassembler::Disassembler()
{
#ifdef _WIN64
    ZydisDecoderInit(&zydisDecoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);
#else
    ZydisDecoderInit(&zydisDecoder, ZYDIS_MACHINE_MODE_LONG_COMPAT_32, ZYDIS_ADDRESS_WIDTH_32);
#endif // _WIN64
}


bool Disassembler::Disasm(unsigned char* code, size_t codesize, Instruction* instruction)
{
    return ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&zydisDecoder, code, codesize, &(instruction->zydisInstrction)));
}

const char* Disassembler::FormatZydisRegister(ZydisRegister reg)
{
    switch (reg)
    {
        case ZYDIS_REGISTER_NONE: return "";
        case ZYDIS_REGISTER_AL: return "al";
        case ZYDIS_REGISTER_CL: return "cl";
        case ZYDIS_REGISTER_DL: return "dl";
        case ZYDIS_REGISTER_BL: return "bl";
        case ZYDIS_REGISTER_AH: return "ah";
        case ZYDIS_REGISTER_CH: return "ch";
        case ZYDIS_REGISTER_DH: return "dh";
        case ZYDIS_REGISTER_BH: return "bh";
        case ZYDIS_REGISTER_SPL: return "spl";
        case ZYDIS_REGISTER_BPL: return "bpl";
        case ZYDIS_REGISTER_SIL: return "sil";
        case ZYDIS_REGISTER_DIL: return "dil";
        case ZYDIS_REGISTER_R8B: return "r8b";
        case ZYDIS_REGISTER_R9B: return "r9b";
        case ZYDIS_REGISTER_R10B: return "r10b";
        case ZYDIS_REGISTER_R11B: return "r11b";
        case ZYDIS_REGISTER_R12B: return "r12b";
        case ZYDIS_REGISTER_R13B: return "r13b";
        case ZYDIS_REGISTER_R14B: return "r14b";
        case ZYDIS_REGISTER_R15B: return "r15b";
        case ZYDIS_REGISTER_AX: return "ax";
        case ZYDIS_REGISTER_CX: return "cx";
        case ZYDIS_REGISTER_DX: return "dx";
        case ZYDIS_REGISTER_BX: return "bx";
        case ZYDIS_REGISTER_SP: return "sp";
        case ZYDIS_REGISTER_BP: return "bp";
        case ZYDIS_REGISTER_SI: return "si";
        case ZYDIS_REGISTER_DI: return "di";
        case ZYDIS_REGISTER_R8W: return "r8w";
        case ZYDIS_REGISTER_R9W: return "r9w";
        case ZYDIS_REGISTER_R10W: return "r10w";
        case ZYDIS_REGISTER_R11W: return "r11w";
        case ZYDIS_REGISTER_R12W: return "r12w";
        case ZYDIS_REGISTER_R13W: return "r13w";
        case ZYDIS_REGISTER_R14W: return "r14w";
        case ZYDIS_REGISTER_R15W: return "r15w";
        case ZYDIS_REGISTER_EAX: return "eax";
        case ZYDIS_REGISTER_ECX: return "ecx";
        case ZYDIS_REGISTER_EDX: return "edx";
        case ZYDIS_REGISTER_EBX: return "ebx";
        case ZYDIS_REGISTER_ESP: return "esp";
        case ZYDIS_REGISTER_EBP: return "ebp";
        case ZYDIS_REGISTER_ESI: return "esi";
        case ZYDIS_REGISTER_EDI: return "edi";
        case ZYDIS_REGISTER_R8D: return "r8d";
        case ZYDIS_REGISTER_R9D: return "r9d";
        case ZYDIS_REGISTER_R10D: return "r10d";
        case ZYDIS_REGISTER_R11D: return "r11d";
        case ZYDIS_REGISTER_R12D: return "r12d";
        case ZYDIS_REGISTER_R13D: return "r13d";
        case ZYDIS_REGISTER_R14D: return "r14d";
        case ZYDIS_REGISTER_R15D: return "r15d";
        case ZYDIS_REGISTER_RAX: return "rax";
        case ZYDIS_REGISTER_RCX: return "rcx";
        case ZYDIS_REGISTER_RDX: return "rdx";
        case ZYDIS_REGISTER_RBX: return "rbx";
        case ZYDIS_REGISTER_RSP: return "rsp";
        case ZYDIS_REGISTER_RBP: return "rbp";
        case ZYDIS_REGISTER_RSI: return "rsi";
        case ZYDIS_REGISTER_RDI: return "rdi";
        case ZYDIS_REGISTER_R8: return "r8";
        case ZYDIS_REGISTER_R9: return "r9";
        case ZYDIS_REGISTER_R10: return "r10";
        case ZYDIS_REGISTER_R11: return "r11";
        case ZYDIS_REGISTER_R12: return "r12";
        case ZYDIS_REGISTER_R13: return "r13";
        case ZYDIS_REGISTER_R14: return "r14";
        case ZYDIS_REGISTER_R15: return "r15";
        case ZYDIS_REGISTER_ST0: return "st0";
        case ZYDIS_REGISTER_ST1: return "st1";
        case ZYDIS_REGISTER_ST2: return "st2";
        case ZYDIS_REGISTER_ST3: return "st3";
        case ZYDIS_REGISTER_ST4: return "st4";
        case ZYDIS_REGISTER_ST5: return "st5";
        case ZYDIS_REGISTER_ST6: return "st6";
        case ZYDIS_REGISTER_ST7: return "st7";
        case ZYDIS_REGISTER_X87CONTROL: return "x87control";
        case ZYDIS_REGISTER_X87STATUS: return "x87status";
        case ZYDIS_REGISTER_X87TAG: return "x87tag";
        case ZYDIS_REGISTER_MM0: return "mm0";
        case ZYDIS_REGISTER_MM1: return "mm1";
        case ZYDIS_REGISTER_MM2: return "mm2";
        case ZYDIS_REGISTER_MM3: return "mm3";
        case ZYDIS_REGISTER_MM4: return "mm4";
        case ZYDIS_REGISTER_MM5: return "mm5";
        case ZYDIS_REGISTER_MM6: return "mm6";
        case ZYDIS_REGISTER_MM7: return "mm7";
        case ZYDIS_REGISTER_XMM0: return "xmm0";
        case ZYDIS_REGISTER_XMM1: return "xmm1";
        case ZYDIS_REGISTER_XMM2: return "xmm2";
        case ZYDIS_REGISTER_XMM3: return "xmm3";
        case ZYDIS_REGISTER_XMM4: return "xmm4";
        case ZYDIS_REGISTER_XMM5: return "xmm5";
        case ZYDIS_REGISTER_XMM6: return "xmm6";
        case ZYDIS_REGISTER_XMM7: return "xmm7";
        case ZYDIS_REGISTER_XMM8: return "xmm8";
        case ZYDIS_REGISTER_XMM9: return "xmm9";
        case ZYDIS_REGISTER_XMM10: return "xmm10";
        case ZYDIS_REGISTER_XMM11: return "xmm11";
        case ZYDIS_REGISTER_XMM12: return "xmm12";
        case ZYDIS_REGISTER_XMM13: return "xmm13";
        case ZYDIS_REGISTER_XMM14: return "xmm14";
        case ZYDIS_REGISTER_XMM15: return "xmm15";
        case ZYDIS_REGISTER_XMM16: return "xmm16";
        case ZYDIS_REGISTER_XMM17: return "xmm17";
        case ZYDIS_REGISTER_XMM18: return "xmm18";
        case ZYDIS_REGISTER_XMM19: return "xmm19";
        case ZYDIS_REGISTER_XMM20: return "xmm20";
        case ZYDIS_REGISTER_XMM21: return "xmm21";
        case ZYDIS_REGISTER_XMM22: return "xmm22";
        case ZYDIS_REGISTER_XMM23: return "xmm23";
        case ZYDIS_REGISTER_XMM24: return "xmm24";
        case ZYDIS_REGISTER_XMM25: return "xmm25";
        case ZYDIS_REGISTER_XMM26: return "xmm26";
        case ZYDIS_REGISTER_XMM27: return "xmm27";
        case ZYDIS_REGISTER_XMM28: return "xmm28";
        case ZYDIS_REGISTER_XMM29: return "xmm29";
        case ZYDIS_REGISTER_XMM30: return "xmm30";
        case ZYDIS_REGISTER_XMM31: return "xmm31";
        case ZYDIS_REGISTER_YMM0: return "ymm0";
        case ZYDIS_REGISTER_YMM1: return "ymm1";
        case ZYDIS_REGISTER_YMM2: return "ymm2";
        case ZYDIS_REGISTER_YMM3: return "ymm3";
        case ZYDIS_REGISTER_YMM4: return "ymm4";
        case ZYDIS_REGISTER_YMM5: return "ymm5";
        case ZYDIS_REGISTER_YMM6: return "ymm6";
        case ZYDIS_REGISTER_YMM7: return "ymm7";
        case ZYDIS_REGISTER_YMM8: return "ymm8";
        case ZYDIS_REGISTER_YMM9: return "ymm9";
        case ZYDIS_REGISTER_YMM10: return "ymm10";
        case ZYDIS_REGISTER_YMM11: return "ymm11";
        case ZYDIS_REGISTER_YMM12: return "ymm12";
        case ZYDIS_REGISTER_YMM13: return "ymm13";
        case ZYDIS_REGISTER_YMM14: return "ymm14";
        case ZYDIS_REGISTER_YMM15: return "ymm15";
        case ZYDIS_REGISTER_YMM16: return "ymm16";
        case ZYDIS_REGISTER_YMM17: return "ymm17";
        case ZYDIS_REGISTER_YMM18: return "ymm18";
        case ZYDIS_REGISTER_YMM19: return "ymm19";
        case ZYDIS_REGISTER_YMM20: return "ymm20";
        case ZYDIS_REGISTER_YMM21: return "ymm21";
        case ZYDIS_REGISTER_YMM22: return "ymm22";
        case ZYDIS_REGISTER_YMM23: return "ymm23";
        case ZYDIS_REGISTER_YMM24: return "ymm24";
        case ZYDIS_REGISTER_YMM25: return "ymm25";
        case ZYDIS_REGISTER_YMM26: return "ymm26";
        case ZYDIS_REGISTER_YMM27: return "ymm27";
        case ZYDIS_REGISTER_YMM28: return "ymm28";
        case ZYDIS_REGISTER_YMM29: return "ymm29";
        case ZYDIS_REGISTER_YMM30: return "ymm30";
        case ZYDIS_REGISTER_YMM31: return "ymm31";
        case ZYDIS_REGISTER_ZMM0: return "zmm0";
        case ZYDIS_REGISTER_ZMM1: return "zmm1";
        case ZYDIS_REGISTER_ZMM2: return "zmm2";
        case ZYDIS_REGISTER_ZMM3: return "zmm3";
        case ZYDIS_REGISTER_ZMM4: return "zmm4";
        case ZYDIS_REGISTER_ZMM5: return "zmm5";
        case ZYDIS_REGISTER_ZMM6: return "zmm6";
        case ZYDIS_REGISTER_ZMM7: return "zmm7";
        case ZYDIS_REGISTER_ZMM8: return "zmm8";
        case ZYDIS_REGISTER_ZMM9: return "zmm9";
        case ZYDIS_REGISTER_ZMM10: return "zmm10";
        case ZYDIS_REGISTER_ZMM11: return "zmm11";
        case ZYDIS_REGISTER_ZMM12: return "zmm12";
        case ZYDIS_REGISTER_ZMM13: return "zmm13";
        case ZYDIS_REGISTER_ZMM14: return "zmm14";
        case ZYDIS_REGISTER_ZMM15: return "zmm15";
        case ZYDIS_REGISTER_ZMM16: return "zmm16";
        case ZYDIS_REGISTER_ZMM17: return "zmm17";
        case ZYDIS_REGISTER_ZMM18: return "zmm18";
        case ZYDIS_REGISTER_ZMM19: return "zmm19";
        case ZYDIS_REGISTER_ZMM20: return "zmm20";
        case ZYDIS_REGISTER_ZMM21: return "zmm21";
        case ZYDIS_REGISTER_ZMM22: return "zmm22";
        case ZYDIS_REGISTER_ZMM23: return "zmm23";
        case ZYDIS_REGISTER_ZMM24: return "zmm24";
        case ZYDIS_REGISTER_ZMM25: return "zmm25";
        case ZYDIS_REGISTER_ZMM26: return "zmm26";
        case ZYDIS_REGISTER_ZMM27: return "zmm27";
        case ZYDIS_REGISTER_ZMM28: return "zmm28";
        case ZYDIS_REGISTER_ZMM29: return "zmm29";
        case ZYDIS_REGISTER_ZMM30: return "zmm30";
        case ZYDIS_REGISTER_ZMM31: return "zmm31";
        case ZYDIS_REGISTER_FLAGS: return "flags";
        case ZYDIS_REGISTER_EFLAGS: return "eflags";
        case ZYDIS_REGISTER_RFLAGS: return "rflags";
        case ZYDIS_REGISTER_IP: return "ip";
        case ZYDIS_REGISTER_EIP: return "eip";
        case ZYDIS_REGISTER_RIP: return "rip";
        case ZYDIS_REGISTER_ES: return "es";
        case ZYDIS_REGISTER_CS: return "cs";
        case ZYDIS_REGISTER_SS: return "ss";
        case ZYDIS_REGISTER_DS: return "ds";
        case ZYDIS_REGISTER_FS: return "fs";
        case ZYDIS_REGISTER_GS: return "gs";
        case ZYDIS_REGISTER_GDTR: return "gdtr";
        case ZYDIS_REGISTER_LDTR: return "ldtr";
        case ZYDIS_REGISTER_IDTR: return "idtr";
        case ZYDIS_REGISTER_TR: return "tr";
        case ZYDIS_REGISTER_TR0: return "tr0";
        case ZYDIS_REGISTER_TR1: return "tr1";
        case ZYDIS_REGISTER_TR2: return "tr2";
        case ZYDIS_REGISTER_TR3: return "tr3";
        case ZYDIS_REGISTER_TR4: return "tr4";
        case ZYDIS_REGISTER_TR5: return "tr5";
        case ZYDIS_REGISTER_TR6: return "tr6";
        case ZYDIS_REGISTER_TR7: return "tr7";
        case ZYDIS_REGISTER_CR0: return "cr0";
        case ZYDIS_REGISTER_CR1: return "cr1";
        case ZYDIS_REGISTER_CR2: return "cr2";
        case ZYDIS_REGISTER_CR3: return "cr3";
        case ZYDIS_REGISTER_CR4: return "cr4";
        case ZYDIS_REGISTER_CR5: return "cr5";
        case ZYDIS_REGISTER_CR6: return "cr6";
        case ZYDIS_REGISTER_CR7: return "cr7";
        case ZYDIS_REGISTER_CR8: return "cr8";
        case ZYDIS_REGISTER_CR9: return "cr9";
        case ZYDIS_REGISTER_CR10: return "cr10";
        case ZYDIS_REGISTER_CR11: return "cr11";
        case ZYDIS_REGISTER_CR12: return "cr12";
        case ZYDIS_REGISTER_CR13: return "cr13";
        case ZYDIS_REGISTER_CR14: return "cr14";
        case ZYDIS_REGISTER_CR15: return "cr15";
        case ZYDIS_REGISTER_DR0: return "dr0";
        case ZYDIS_REGISTER_DR1: return "dr1";
        case ZYDIS_REGISTER_DR2: return "dr2";
        case ZYDIS_REGISTER_DR3: return "dr3";
        case ZYDIS_REGISTER_DR4: return "dr4";
        case ZYDIS_REGISTER_DR5: return "dr5";
        case ZYDIS_REGISTER_DR6: return "dr6";
        case ZYDIS_REGISTER_DR7: return "dr7";
        case ZYDIS_REGISTER_DR8: return "dr8";
        case ZYDIS_REGISTER_DR9: return "dr9";
        case ZYDIS_REGISTER_DR10: return "dr10";
        case ZYDIS_REGISTER_DR11: return "dr11";
        case ZYDIS_REGISTER_DR12: return "dr12";
        case ZYDIS_REGISTER_DR13: return "dr13";
        case ZYDIS_REGISTER_DR14: return "dr14";
        case ZYDIS_REGISTER_DR15: return "dr15";
        case ZYDIS_REGISTER_K0: return "k0";
        case ZYDIS_REGISTER_K1: return "k1";
        case ZYDIS_REGISTER_K2: return "k2";
        case ZYDIS_REGISTER_K3: return "k3";
        case ZYDIS_REGISTER_K4: return "k4";
        case ZYDIS_REGISTER_K5: return "k5";
        case ZYDIS_REGISTER_K6: return "k6";
        case ZYDIS_REGISTER_K7: return "k7";
        case ZYDIS_REGISTER_BND0: return "bnd0";
        case ZYDIS_REGISTER_BND1: return "bnd1";
        case ZYDIS_REGISTER_BND2: return "bnd2";
        case ZYDIS_REGISTER_BND3: return "bnd3";
        case ZYDIS_REGISTER_BNDCFG: return "bndcfg";
        case ZYDIS_REGISTER_BNDSTATUS: return "bndstatus";
        case ZYDIS_REGISTER_MXCSR: return "mxcsr";
        case ZYDIS_REGISTER_PKRU: return "pkru";
        case ZYDIS_REGISTER_XCR0: return "xcr0";
    }
    return "";
}


void Disassembler::loadPage(void* buffer, size_t pageAddr)
{
    if (m_pageInstrAddrMap.count(pageAddr) != 0) {
        return;
    }


    std::vector<size_t> addrList;
    std::map<size_t, int> indexMap;

    Instruction instr;
    int index = 0;
    size_t position = 0;
    while (position < 0x1000)
    {
        if (Disasm((unsigned char*)buffer + position, 0x1000 - position, &instr) == false) {
            break;
        }
        instr.zydisInstrction.length;

        addrList.push_back(pageAddr + position);
        indexMap.insert(std::make_pair(pageAddr + position, index));

        index++;
        position += instr.zydisInstrction.length;
    }

    m_pageInstrAddrMap.insert(std::make_pair(pageAddr, addrList));
    m_pageInstrAddrIndexMap.insert(std::make_pair(pageAddr, indexMap));
}

void Disassembler::Reset()
{

}
