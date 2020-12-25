#include "Hooks.h"

#define PRINT_PREFIX "[Hooks] "
#include "LDasm.h"
#include <ntdef.h>
#include <intrin.h>

/*
00000 | 68 88776655        | push 55667788                          |
00005 | C74424 04 44332211 | mov dword ptr ss:[rsp+4],11223344      |
0000D | C3                 | ret                                    |
*/
#define PUSHRET14_CODE {0x68,0x88,0x77,0x66,0x55,0xC7,0x44,0x24,0x04,0x44,0x33,0x22,0x11,0xC3}
#define PUSHRET14_JMPADDR_LOW 9
#define PUSHRET14_JMPADDR_HIGH 1

/*
0000 | 48:B8 8877665544332211          | mov rax,1122334455667788               |
000A | FFE0                            | jmp rax                                |
 */
#define JMPRAX12_CODE {0x48, 0xB8, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0xFF, 0xE0}
#define JMPRAX12_CODE_JMPADDR 0x2

 /*
 00007FF692980000 | 41:57                           | push r15                               |
 00007FF692980002 | 41:56                           | push r14                               | r14:"minkernel\\ntdll\\ldrinit.c"
 00007FF692980004 | 41:55                           | push r13                               |
 00007FF692980006 | 41:54                           | push r12                               |
 00007FF692980008 | 41:53                           | push r11                               |
 00007FF69298000A | 41:52                           | push r10                               |
 00007FF69298000C | 41:51                           | push r9                                |
 00007FF69298000E | 41:50                           | push r8                                |
 00007FF692980010 | 57                              | push rdi                               | rdi:PEB.InheritedAddressSpace
 00007FF692980011 | 56                              | push rsi                               | rsi:"LdrpInitializeProcess"
 00007FF692980012 | 55                              | push rbp                               |
 00007FF692980013 | 48:8D7424 58                    | lea rsi,qword ptr ss:[rsp+58]          |
 00007FF692980018 | 56                              | push rsi                               | dummy for rsp
 00007FF692980019 | 53                              | push rbx                               |
 00007FF69298001A | 52                              | push rdx                               |
 00007FF69298001B | 51                              | push rcx                               | rcx:sub_7FFACE90B1AC+88
 00007FF69298001C | 50                              | push rax                               |
 00007FF69298001D | 48:8D0C24                       | lea rcx,qword ptr ss:[rsp]             | rcx:sub_7FFACE90B1AC+88
 00007FF692980021 | 48:B8 EFCDAB8967452301          | mov rax,123456789ABCDEF                |
 00007FF69298002B | 48:83EC 08                      | sub rsp,8                              |
 00007FF69298002F | FFD0                            | call rax                               |
 00007FF692980031 | 48:83C4 08                      | add rsp,8                              |
 00007FF692980035 | 58                              | pop rax                                |
 00007FF692980036 | 59                              | pop rcx                                | rcx:sub_7FFACE90B1AC+88
 00007FF692980037 | 5A                              | pop rdx                                |
 00007FF692980038 | 5B                              | pop rbx                                |
 00007FF692980039 | 48:83C4 08                      | add rsp,8                              |
 00007FF69298003D | 5D                              | pop rbp                                |
 00007FF69298003E | 5E                              | pop rsi                                | rsi:"LdrpInitializeProcess"
 00007FF69298003F | 5F                              | pop rdi                                | rdi:PEB.InheritedAddressSpace
 00007FF692980040 | 41:58                           | pop r8                                 |
 00007FF692980042 | 41:59                           | pop r9                                 |
 00007FF692980044 | 41:5A                           | pop r10                                |
 00007FF692980046 | 41:5B                           | pop r11                                |
 00007FF692980048 | 41:5C                           | pop r12                                |
 00007FF69298004A | 41:5D                           | pop r13                                |
 00007FF69298004C | 41:5E                           | pop r14                                | r14:"minkernel\\ntdll\\ldrinit.c"
 00007FF69298004E | 41:5F                           | pop r15                                |
 */
#define TEMPLATE_CODE {0x41, 0x57, 0x41, 0x56, 0x41, 0x55, 0x41, 0x54, 0x41, 0x53, 0x41, 0x52, 0x41, 0x51, 0x41, 0x50, 0x57, 0x56, 0x55, 0x48, 0x8D, 0x74, 0x24, 0x58, 0x56, 0x53, 0x52, 0x51, 0x50, 0x48, 0x8D, 0x0C, 0x24, 0x48, 0xB8, 0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01, 0x48, 0x83, 0xEC, 0x08, 0xFF, 0xD0, 0x48, 0x83, 0xC4, 0x08, 0x58, 0x59, 0x5A, 0x5B, 0x48, 0x83, 0xC4, 0x08, 0x5D, 0x5E, 0x5F, 0x41, 0x58, 0x41, 0x59, 0x41, 0x5A, 0x41, 0x5B, 0x41, 0x5C, 0x41, 0x5D, 0x41, 0x5E, 0x41, 0x5F}
#define TEMPLATE_CODE_HANDLE  0x23


KIRQL WPOFFx64()
{
    KIRQL  irql = KeRaiseIrqlToDpcLevel();
    UINT64 cr0 = __readcr0();
    cr0 &= 0xfffffffffffeffff;
    __writecr0(cr0);
    _disable();
    return irql;
}

void WPONx64(KIRQL irql)
{
    UINT64 cr0 = __readcr0();
    cr0 |= 0x10000;
    _enable();
    __writecr0(cr0);
    KeLowerIrql(irql);
}


NTSTATUS InsertIntercept64(__in PVOID SourceCode, __in P_HANDLE HandleRoutine, __in PINTERCEPT_CREATE lpCreateInfo, __out PINTERCEPT_INFO lpInterceptInfo)
{
    KdPrint((PRINT_PREFIX "SourceCode:%p\n", SourceCode));
    KdPrint((PRINT_PREFIX "HandleRoutine:%p\n", HandleRoutine));


    // 检查目标源代码地址是否可用.
    if (!MmIsAddressValid(SourceCode))
    {
        KdPrint((PRINT_PREFIX "SourceCode 是无效地址!\n"));
        return STATUS_UNSUCCESSFUL;
    }

    // hook最小需要的大小.
    ULONG patchNeedSize = 0;
    switch (lpCreateInfo->patchType)
    {
        case PUSH_RET_14:
            patchNeedSize = 14;
            break;
        case MRAX_JMP_12:
            patchNeedSize = 12;
            break;
        default:
            return STATUS_UNSUCCESSFUL;
    }

    // 计算Hook所需的大小.
    ULONG hookSize = 0;
    while (hookSize < patchNeedSize)
    {
        ldasm_data ld;
        PUCHAR     p = (PUCHAR)SourceCode + hookSize;
        ULONG      length = ldasm(p, &ld, 1);
        hookSize += length;
    }

    UCHAR code_template[] = TEMPLATE_CODE;
    UCHAR code_pushret14[] = PUSHRET14_CODE;

    // 计算各种内存块大小
    ULONG templateCodeSize = sizeof(code_template);
    ULONG preCodeSize = lpCreateInfo->preCodeSize;
    ULONG orignalCodeSize = hookSize;                                                          // 被覆盖的原始代码大小
    ULONG returnCodeSize = sizeof(code_pushret14);                                            // 用于跳转回原处的代码大小
    ULONG interceptCodeSize = templateCodeSize + preCodeSize + orignalCodeSize + returnCodeSize; // 拦截代码

    // 申请内存
    PVOID interceptCode = ExAllocatePoolWithTag(NonPagedPool, interceptCodeSize, 'KOOH');
    if (interceptCode == NULL)
    {
        KdPrint((PRINT_PREFIX "AllocatePool Failed!\n"));
        return STATUS_UNSUCCESSFUL;
    }
    RtlFillMemory(interceptCode, interceptCodeSize, 0x90);

    // 计算指针
    PUCHAR templateCode = (PUCHAR)interceptCode;
    PUCHAR preCode = (PUCHAR)interceptCode + templateCodeSize;
    PUCHAR orginalCode = (PUCHAR)interceptCode + templateCodeSize + preCodeSize;
    PUCHAR returnCode = (PUCHAR)interceptCode + templateCodeSize + preCodeSize + orignalCodeSize;

    // 部署interceptCode代码
    memcpy(templateCode, code_template, templateCodeSize);
    memcpy(preCode, lpCreateInfo->preCode, preCodeSize);
    memcpy(orginalCode, SourceCode, orignalCodeSize);
    memcpy(returnCode, code_pushret14, returnCodeSize);

    // 修复Hnadle地址
    memcpy(templateCode + TEMPLATE_CODE_HANDLE, &HandleRoutine, sizeof(ULONG64));

    // 修复returnCode地址
    LARGE_INTEGER lagerInteger;
    lagerInteger.QuadPart = (ULONG64)SourceCode + hookSize;
    memcpy(returnCode + PUSHRET14_JMPADDR_LOW, &lagerInteger.HighPart, sizeof(ULONG));
    memcpy(returnCode + PUSHRET14_JMPADDR_HIGH, &lagerInteger.LowPart, sizeof(ULONG));

    PVOID code_hookcode = NULL;
    // 准备hookCode 并修复硬编码地址
    switch (lpCreateInfo->patchType)
    {
        case PUSH_RET_14:
        {
            UCHAR codeBuffer[] = PUSHRET14_CODE;
            lagerInteger.QuadPart = (ULONG64)interceptCode;
            memcpy(codeBuffer + PUSHRET14_JMPADDR_LOW, &lagerInteger.HighPart, sizeof(ULONG));
            memcpy(codeBuffer + PUSHRET14_JMPADDR_HIGH, &lagerInteger.LowPart, sizeof(ULONG));
            code_hookcode = codeBuffer;
            break;
        }
        case MRAX_JMP_12:
        {
            UCHAR codeBuffer[] = JMPRAX12_CODE;
            memcpy(codeBuffer + JMPRAX12_CODE_JMPADDR, &interceptCode, sizeof(ULONG64));
            code_hookcode = codeBuffer;
            break;
        }
        default:
            return STATUS_UNSUCCESSFUL;
    }




    // 部署hookCode
    PUSHORT DestHeader = (PUSHORT)SourceCode;
    PUSHORT CodeHeader = (PUSHORT)code_hookcode;
    PUCHAR DestTail = (PUCHAR)SourceCode + 2;
    PUCHAR CodeTail = (PUCHAR)code_hookcode + 2;
    SIZE_T TailSize = patchNeedSize - 2;

    // 更新代码
    KIRQL irql = WPOFFx64();
    *DestHeader = 0xFEEB;
    memcpy(DestTail, CodeTail, TailSize);
    memset((PUCHAR)SourceCode + patchNeedSize, 0x90, hookSize - patchNeedSize);
    *DestHeader = *CodeHeader;
    WPONx64(irql);

    //UpdateCode(SourceCode, code_hookcode, patchNeedSize);
    //KIRQL irql = WPOFFx64();
    //memcpy(SourceCode, code_hookcode, patchNeedSize); // 修改代码
    //memset((PUCHAR)SourceCode + patchNeedSize,        // 填充空隙
    //    0x90,
    //    hookSize - patchNeedSize
    //);
    //WPONx64(irql);

    if (lpInterceptInfo)
    {
        lpInterceptInfo->hookFlag = TRUE;

        lpInterceptInfo->orignalCode = orginalCode;
        lpInterceptInfo->orignalCodeSize = orignalCodeSize;

        lpInterceptInfo->sourceCode = SourceCode;
        lpInterceptInfo->intercetCode = interceptCode;
        lpInterceptInfo->intercetCodeSize = interceptCodeSize;
    }

    KdPrint((PRINT_PREFIX "InterceptCode:%p HookSize:%ld\n", interceptCode, hookSize));
    return STATUS_SUCCESS;
}

NTSTATUS RemoveIntercept(PINTERCEPT_INFO interceptInfo)
{
    if (interceptInfo->hookFlag == FALSE)
    {
        return STATUS_SUCCESS;
    }

    PVOID SourceCode = interceptInfo->sourceCode;
    PVOID InterceptCode = interceptInfo->intercetCode;
    ULONG OrignalCodeSize = interceptInfo->orignalCodeSize;

    if (!MmIsAddressValid(SourceCode))
    {
        KdPrint((PRINT_PREFIX "SourceCode is invaild! SourceCode=%p \n", SourceCode));
        return STATUS_UNSUCCESSFUL;
    }

    if (!MmIsAddressValid(InterceptCode))
    {
        KdPrint((PRINT_PREFIX "InterceptCode is invaild! InterceptCode=%p \n", InterceptCode));
        return STATUS_UNSUCCESSFUL;
    }

    PVOID OrignalCode = interceptInfo->orignalCode;

    // 还原代码
    //KIRQL irql = WPOFFx64();
    //memcpy(SourceCode, OrignalCode, OrignalCodeSize);
    //WPONx64(irql);

    KIRQL irql = WPOFFx64();
    *(PUSHORT)SourceCode = 0xFEEB;
    memcpy(PUCHAR(SourceCode) + 2, (PUCHAR)OrignalCode +2, OrignalCodeSize - 2);
    *(PUSHORT)SourceCode = *(PUSHORT)OrignalCode;
    WPONx64(irql);


    // 释放内存
    //ExFreePool(InterceptCode);

    return STATUS_SUCCESS;
}

