#pragma once

#include <ntddk.h>

#pragma warning(disable:4201)
typedef struct _REGISTER_CONTEXT
{
    union
    {
        struct
        {
            ULONG eax;
            ULONG ecx;
            ULONG edx;
            ULONG ebx;
            ULONG esp;
            ULONG ebp;
            ULONG esi;
            ULONG edi;
        } reg32;

        struct
        {
            ULONG64 rax;
            ULONG64 rcx;
            ULONG64 rdx;
            ULONG64 rbx;
            ULONG64 rsp;
            ULONG64 rbp;
            ULONG64 rsi;
            ULONG64 rdi;
            ULONG64 r8;
            ULONG64 r9;
            ULONG64 r10;
            ULONG64 r11;
            ULONG64 r12;
            ULONG64 r13;
            ULONG64 r14;
            ULONG64 r15;
        } reg64;
    };
} REGISTER_CONTEXT, *PREGISTER_CONTEXT;


typedef struct _INTERCEPT_INFO
{
    BOOLEAN hookFlag;

    ULONG orignalCodeSize;
    ULONG intercetCodeSize;

    PVOID sourceCode;
    PVOID orignalCode;
    PVOID intercetCode;
} INTERCEPT_INFO, *PINTERCEPT_INFO;

enum INTERCEPT_HOOK_TYPE
{
    PUSH_RET_14,
    MRAX_JMP_12
};

typedef struct _INTERCEPT_CREATE
{
    ULONG patchType;

    PVOID preCode;
    ULONG preCodeSize;
} INTERCEPT_CREATE, *PINTERCEPT_CREATE;

typedef VOID (NTAPI* P_HANDLE)(PREGISTER_CONTEXT Context);

KIRQL WPOFFx64();
void WPONx64(KIRQL irql);
/*
 * 使用inline-hook再目标代码位置插入代码. 让其可以执行HandleRoutine
 * FBI WARNING: 没有处理多核并行的情况, 极端情况下可能导致蓝屏. 
 *
 * 名词解释:
 * HookCode: 用于替换源代码的跳转指令(push xx;  mov [rsp+4], xx;   ret) 固定大小为14字节.
 * OriginalCode: 被替换的原始指令. Size >= 14
 * TemplateCode: HookCode需要跳转到该位置. 主要做3件事: pushad / call handle  / popad
 * 
 * 参数: 
 * SourceCode: 需要插入的代码位置. 注意:需要保证14+字节的代码内,没有使用相对地址硬编码.
 * HandleRoutine: hook成功后. 每次都会调用这个例程.
 */
NTSTATUS InsertIntercept64(__in PVOID SourceCode,__in P_HANDLE HandleRoutine,__in PINTERCEPT_CREATE lpCreateInfo,__out PINTERCEPT_INFO lpInterceptInfo);

/*
 * 还原被hook的代码
 */
NTSTATUS RemoveIntercept(PINTERCEPT_INFO interceptInfo);

