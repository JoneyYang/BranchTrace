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
 * ʹ��inline-hook��Ŀ�����λ�ò������. �������ִ��HandleRoutine
 * FBI WARNING: û�д����˲��е����, ��������¿��ܵ�������. 
 *
 * ���ʽ���:
 * HookCode: �����滻Դ�������תָ��(push xx;  mov [rsp+4], xx;   ret) �̶���СΪ14�ֽ�.
 * OriginalCode: ���滻��ԭʼָ��. Size >= 14
 * TemplateCode: HookCode��Ҫ��ת����λ��. ��Ҫ��3����: pushad / call handle  / popad
 * 
 * ����: 
 * SourceCode: ��Ҫ����Ĵ���λ��. ע��:��Ҫ��֤14+�ֽڵĴ�����,û��ʹ����Ե�ַӲ����.
 * HandleRoutine: hook�ɹ���. ÿ�ζ�������������.
 */
NTSTATUS InsertIntercept64(__in PVOID SourceCode,__in P_HANDLE HandleRoutine,__in PINTERCEPT_CREATE lpCreateInfo,__out PINTERCEPT_INFO lpInterceptInfo);

/*
 * ��ԭ��hook�Ĵ���
 */
NTSTATUS RemoveIntercept(PINTERCEPT_INFO interceptInfo);

