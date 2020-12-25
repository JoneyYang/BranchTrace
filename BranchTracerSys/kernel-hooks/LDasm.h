#ifndef _LDASM_
#define _LDASM_

#include <ntddk.h>

#ifdef _WIN64
#define is_x64 1
#else
#define is_x64 0
#endif//USE64

#ifdef __cplusplus
extern "C"
{
#endif

#define F_INVALID       0x01
#define F_PREFIX        0x02
#define F_REX           0x04
#define F_MODRM         0x08
#define F_SIB           0x10
#define F_DISP          0x20
#define F_IMM           0x40
#define F_RELATIVE      0x80

typedef struct _ldasm_data
{
    UINT8 flags;
    UINT8 rex;
    UINT8 modrm;
    UINT8 sib;
    UINT8 opcd_offset;
    UINT8 opcd_size;
    UINT8 disp_offset;
    UINT8 disp_size;
    UINT8 imm_offset;
    UINT8 imm_size;
} ldasm_data;

unsigned int __fastcall ldasm(void* code, ldasm_data* ld, UINT32 is64);
unsigned long __fastcall SizeOfProc(void* Proc);
void* __fastcall ResolveJmp(void* Proc);

#ifdef __cplusplus
}
#endif

#endif//_LDASM_
