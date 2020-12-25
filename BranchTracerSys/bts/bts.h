#pragma once
#include <ntdef.h>


typedef struct _DS_AREA64 {
    /*BTS*/
    ULONG64 BTSBufferBase;
    ULONG64 BTSIndex;
    ULONG64 BTSAbsoluteMaximum;
    ULONG64 BTSInterruptThreshold;

    /*PEBS*/
    ULONG64 PEBSBufferBase;
    ULONG64 PEBSIndex;
    ULONG64 PEBSMaximum;
    ULONG64 PEBSThreshold;
    ULONG64 counter0;
    ULONG64 counter1;
    ULONG64 reserved;

}DS_AREA64, * PDS_AREA64;

typedef struct _DS_AREA32 {
    /*BTS*/
    ULONG BTSBufferBase;
    ULONG BTSIndex;
    ULONG BTSMaximum;
    ULONG BTSThreshold;

    /*PEBS*/
    ULONG PEBSBufferBase;
    ULONG PEBSIndex;
    ULONG PEBSMaximum;
    ULONG PEBSThreshold;
    ULONG counter0;
    ULONG counter1;
    ULONG counter2;
    ULONG counter3;
    ULONG reserved;

}DS_AREA32, * PDS_AREA32;


typedef struct _BTS_RECORD64
{
    ULONG64 from;   // 记录分支的源地址
    ULONG64 to;     // 记录分支的目标地址
    ULONG64 branchPredicted;
}BTS_RECORD64, * PBTS_RECORD64;

//typedef struct _BTS_RECORD32
//{
//    ULONG32 from;   // 记录分支的源地址
//    ULONG32 to;     // 记录分支的目标地址
//    ULONG32 branchPredicted;
//}BTS_RECORD32, * PBTS_RECORD32;


typedef struct _PEBS_RECORD64
{
    ULONG64 rflags;
    ULONG64 rip;
    ULONG64 rax;
    ULONG64 rbx;
    ULONG64 rcx;
    ULONG64 rdx;
    ULONG64 rsi;
    ULONG64 rdi;
    ULONG64 rbp;
    ULONG64 rsp;
    ULONG64 r8;
    ULONG64 r9;
    ULONG64 r10;
    ULONG64 r11;
    ULONG64 r12;
    ULONG64 r13;
    ULONG64 r14;
    ULONG64 r15;
}PEBS_RECORD64,*PPEBS_RECORD64;

typedef struct _PEBS_RECORD32
{
    ULONG eflags;
    ULONG eip;
    ULONG eax;
    ULONG ebx;
    ULONG ecx;
    ULONG edx;
    ULONG esi;
    ULONG edi;
    ULONG ebp;
    ULONG esp;
}PEBS_RECORD32, * PPEBS_RECORD32;



VOID InitializeEnvironment();
VOID ClearThreadTraceData();
BOOLEAN ReadThreadTraceData(HANDLE ThreadId, PVOID Buffer, SIZE_T BufferSize, ULONG_PTR FilterStart, ULONG_PTR FilterEnd, OUT SIZE_T* lpReadSize);
BOOLEAN StartMonitThread(HANDLE ThreadId);
VOID StopMonitThread(HANDLE ThreadId);
NTSTATUS SetupBTS();
VOID ResetBTS();
