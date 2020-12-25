#pragma once
#pragma warning(disable 26812)
#include <Zydis/Decoder.h>
#include <map>
#include <set>
#include <vector>

struct Instruction {
    ZydisDecodedInstruction zydisInstrction;
};

class Disassembler {

private:
    ZydisDecoder zydisDecoder;
    std::map<size_t, std::vector<size_t>> m_pageInstrAddrMap;
    std::map<size_t, std::map<size_t, int>> m_pageInstrAddrIndexMap;



public:
    Disassembler();

    bool Disasm(unsigned char* code, size_t codesize, Instruction* instrction);
    const char* FormatZydisRegister(ZydisRegister reg);

    void loadPage(void* buffer, size_t pageAddr);

    void Reset();
};
