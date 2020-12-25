#include "../kernel-tools/KernelBase.h"
#include "bts.h"
#include <intrin.h>
#include "../asm/debug.h"
#include "../kernel-hooks/Hooks.h"


typedef struct _THREAD_TRACE_DATA
{
    LIST_ENTRY ListEntry;
    HANDLE ThreadId;
    HANDLE ProcessId;
    PBTS_RECORD64 Start;
    PBTS_RECORD64 End;
}THREAD_TRACE_DATA, * PTHREAD_TRACE_DATA;




typedef NTSTATUS(__stdcall* KeSetAffinityThread)(PKTHREAD thread, KAFFINITY affinity);

PDS_AREA64 g_DSAreaArr[64] = { 0 };
PBTS_RECORD64 g_BtsBufferArr[64] = { 0 };
//PPEBS_RECORD64 g_PebsBufferArr[64] = { 0 };

ULONG ProcessorCount = 0;
KAFFINITY ActivedProcessors = 0;
KeSetAffinityThread KeSetAffinityThreadObj = NULL;
KAFFINITY AllayedProcessor = 0;

#define MAX_CPU 64

INTERCEPT_INFO InterceptInfo;
UCHAR ProcessorFlag[MAX_CPU] = { 0 };
LIST_ENTRY ThreadTraceDataHeaderArr[MAX_CPU];
ULONG_PTR MonitThreadId = 0xFFFFFFFF;




VOID InitializeEnvironment() {
    for (size_t i = 0; i < MAX_CPU; i++)
    {
        InitializeListHead(&ThreadTraceDataHeaderArr[i]);
    }

    // 获取KeSetAffinityThreadStr
    UNICODE_STRING KeSetAffinityThreadStr = RTL_CONSTANT_STRING(L"KeSetAffinityThread");
    KeSetAffinityThreadObj = (KeSetAffinityThread)MmGetSystemRoutineAddress(&KeSetAffinityThreadStr);

    // CPU核心数量
    ProcessorCount = KeQueryActiveProcessorCount(&ActivedProcessors);
}




VOID ClearThreadTraceData() {
    PETHREAD Thread = PsGetCurrentThread();
    for (size_t i = 0; i < MAX_CPU; i++)
    {
        PLIST_ENTRY header = &ThreadTraceDataHeaderArr[i];

        // 切换线程为对应的CPU
        KAFFINITY CurrentAffinity = (KAFFINITY)1 << i;
        if ((AllayedProcessor & CurrentAffinity) == 0) {
            continue;
        }
        KeSetAffinityThreadObj(Thread, CurrentAffinity);

        while (!IsListEmpty(header)) {
            THREAD_TRACE_DATA* s = CONTAINING_RECORD(RemoveTailList(header), THREAD_TRACE_DATA, ListEntry);
            ExFreePool(s->Start);
            ExFreePool(s);
        }

        KeSetAffinityThreadObj(Thread, ActivedProcessors);
    }
}


BOOLEAN ReadThreadTraceData(HANDLE ThreadId, PVOID Buffer, SIZE_T BufferSize, ULONG_PTR FilterStart, ULONG_PTR FilterEnd, OUT SIZE_T* lpReadSize)
{
    if (BufferSize < sizeof(BTS_RECORD64)) {
        return FALSE;
    }

    PBTS_RECORD64 pBuf = (PBTS_RECORD64)Buffer;
    PBTS_RECORD64 end = (PBTS_RECORD64)((PUCHAR)Buffer + BufferSize - sizeof(BTS_RECORD64));

    PETHREAD Thread = PsGetCurrentThread();
    // 遍历每个CPU核心
    for (size_t i = 0; i < MAX_CPU; i++)
    {
        PLIST_ENTRY header = &ThreadTraceDataHeaderArr[i];

        // 切换线程为对应的CPU
        KAFFINITY CurrentAffinity = (KAFFINITY)1 << i;
        if ((AllayedProcessor & CurrentAffinity) == 0) {
            continue;
        }
        KeSetAffinityThreadObj(Thread, CurrentAffinity);

        // 每个CPU都有一串链表, 链表中的每一项记录了线程在一段时间片中的Branch
        PLIST_ENTRY listEntry = header->Blink;
        while (listEntry != header)
        {
            THREAD_TRACE_DATA* traceData = CONTAINING_RECORD(listEntry, THREAD_TRACE_DATA, ListEntry);

            if (traceData->ThreadId != ThreadId) { continue; }
            if (traceData->End <= traceData->Start) { continue; }

            // 逆序复制数据
            PBTS_RECORD64 pData = traceData->End - 1;
            while (pData >= traceData->Start)
            {
                if (
                    (pData->from >= FilterStart && pData->from <= FilterEnd)
                    && (pData->to >= FilterStart && pData->to <= FilterEnd))
                {
                    //DbgPrint("p : %p  data:%p\n", p, data);
                    RtlCopyMemory(pBuf, pData, sizeof(BTS_RECORD64));
                    pBuf++;
                }
                if (pBuf >= end) { break; }

                pData--;
            }

            listEntry = listEntry->Blink;
            if (pBuf >= end) { break; }
        }

        if (pBuf >= end) { break; }

    }

    KeSetAffinityThreadObj(Thread, ActivedProcessors);
    if (lpReadSize) {
        *lpReadSize = (ULONG_PTR)pBuf - (ULONG_PTR)Buffer;
    }

    return TRUE;
}



/* 开启针对线程的监控 */
BOOLEAN StartMonitThread(HANDLE ThreadId) {

    PETHREAD Thread = NULL;
    if (!NT_SUCCESS(PsLookupThreadByThreadId(ThreadId, &Thread))) {
        return FALSE;
    }

    if (MonitThreadId != 0xFFFFFFFF) {
        StopMonitThread((HANDLE)MonitThreadId);
    }

    MonitThreadId = (ULONG_PTR)ThreadId;

    KAFFINITY Affinity = (KAFFINITY)1;
    KeSetAffinityThreadObj((PKTHREAD)Thread, Affinity);
    ObDereferenceObject(Thread);
    return TRUE;
}

/* 停止针对线程的监控 */
VOID StopMonitThread(HANDLE ThreadId) {

    if (ThreadId != NULL) {

        PETHREAD Thread = NULL;
        if (NT_SUCCESS(PsLookupThreadByThreadId(ThreadId, &Thread))) {
            KeSetAffinityThreadObj((PKTHREAD)Thread, ActivedProcessors);
            ObDereferenceObject(Thread);
        }

    }

    KdPrint(("Stop Monitor Thread!\n"));

    MonitThreadId = 0xFFFFFFFF;
}


VOID NTAPI SwapContextHandler(PREGISTER_CONTEXT Context) {
    UNREFERENCED_PARAMETER(Context);

    ULONG CurrentProcessorIndex = KeGetCurrentProcessorIndex();

    // 当前Thread
    PETHREAD CurrentThread = (PETHREAD)Context->reg64.rdi;
    HANDLE CurrentThreadId = PsGetThreadId(CurrentThread);
    HANDLE CurrentProcessId = PsGetThreadProcessId(CurrentThread);

    // 待切换的Thread
    PETHREAD NextThread = (PETHREAD)Context->reg64.rsi;
    HANDLE NextThreadId = PsGetThreadId(NextThread);
    //HANDLE NextProcessId = PsGetThreadProcessId(NextThread);


    // 在线程切换前 将BTS转储到目标空间。
    if (ProcessorFlag[CurrentProcessorIndex] == TRUE) {
        DisableBTS();
        ProcessorFlag[CurrentProcessorIndex] = FALSE;

        PDS_AREA64 DsArea = g_DSAreaArr[CurrentProcessorIndex];

        if (DsArea->BTSIndex != DsArea->BTSBufferBase) {

            // 另外分配一片空间 存放代码
            ULONG64 BufferSize = DsArea->BTSIndex - DsArea->BTSBufferBase;
            PVOID allocate = ExAllocatePoolWithTag(NonPagedPool, (size_t)BufferSize, '.STB');

            // !!!!  这行代码运行时间最长，卡顿基本都是因为这个造成的。
            memcpy(allocate, (PVOID)DsArea->BTSBufferBase, (size_t)BufferSize);

            THREAD_TRACE_DATA* s = (THREAD_TRACE_DATA*)ExAllocatePoolWithTag(NonPagedPool, sizeof(THREAD_TRACE_DATA), '.STB');
            s->Start = (PBTS_RECORD64)allocate;
            s->End = (PBTS_RECORD64)((PUCHAR)allocate + BufferSize);
            s->ThreadId = CurrentThreadId;
            s->ProcessId = CurrentProcessId;
            InsertTailList(&ThreadTraceDataHeaderArr[CurrentProcessorIndex], &s->ListEntry);

            // 重置 Index
            DsArea->BTSIndex = DsArea->BTSBufferBase;
        }
    }


    // 根据线程ID开启 BTS
    if ((ULONG_PTR)NextThreadId == MonitThreadId) {
        EnableBTS();
        ProcessorFlag[CurrentProcessorIndex] = TRUE;
    }
    return;
}


NTSTATUS SetupBTS() {

    // 检查BTS
    DbgPrint("Support DS: %s\n", CheckSupportDS() ? "yes" : "no");
    DbgPrint("Support DS64: %s\n", CheckSupportDS64() ? "yes" : "no");
    DbgPrint("Avaliable BTS: %s\n", CheckAvaliableBTS() ? "yes" : "no");
    DbgPrint("Avaliable PEBS: %s\n", CheckAvaliablePEBS() ? "yes" : "no");
    if (!CheckSupportDS() && !CheckSupportDS64() && !CheckAvaliableBTS() && !CheckAvaliablePEBS()) {
        DbgPrint("BTS Not Support !\n");
        return STATUS_UNSUCCESSFUL;
    }

    if (KeSetAffinityThreadObj == NULL) {
        DbgPrint("Not found KeSetAffinityThread Routine!\n");
        return STATUS_UNSUCCESSFUL;
    }


    PKTHREAD Thread = KeGetCurrentThread();
    ObReferenceObjectByPointer(Thread, 0, *PsThreadType, KernelMode);

    UCHAR Flag = TRUE;
    for (size_t i = 0; i < ProcessorCount; i++)
    {
        KAFFINITY CurrentAffinity = (KAFFINITY)1 << i;
        if ((ActivedProcessors & CurrentAffinity) == 0) {
            continue;
        }

        PDS_AREA64 DebugStoreArea = NULL;
        PBTS_RECORD64 BtsBuffer = NULL;

#define BUFFER_SIZE  0x100000

        // 分配DebugStoreArea
        DebugStoreArea = (PDS_AREA64)ExAllocatePoolWithTag(NonPagedPool, sizeof(DS_AREA64), 'STB.');
        if (DebugStoreArea == NULL) { goto _label_clear; }
        memset(DebugStoreArea, 0, sizeof(DS_AREA64));

        // 分配BTS buffer
        BtsBuffer = (PBTS_RECORD64)ExAllocatePoolWithTag(NonPagedPool, BUFFER_SIZE, 'STB.');
        if (BtsBuffer == NULL) { goto _label_clear; }
        memset(BtsBuffer, 0, BUFFER_SIZE);
        ULONG BtsRecordCount = BUFFER_SIZE / sizeof(BTS_RECORD64);
        //ULONG BtsRecordCount = 10;
        PBTS_RECORD64 BtsAbsoluteMaximum = BtsBuffer + BtsRecordCount;
        PBTS_RECORD64 BtsInterruptThreshold = BtsBuffer + BtsRecordCount + 1;

        DebugStoreArea->BTSBufferBase = (ULONG64)BtsBuffer;
        DebugStoreArea->BTSIndex = (ULONG64)BtsBuffer;
        DebugStoreArea->BTSAbsoluteMaximum = (ULONG64)BtsAbsoluteMaximum;
        DebugStoreArea->BTSInterruptThreshold = (ULONG64)BtsInterruptThreshold;

        DbgPrint("Processor[%d]  Debug Store Area:%p\n", i, DebugStoreArea);

        // 切换CPU
        KeSetAffinityThreadObj(Thread, CurrentAffinity);
        _disable();
        SetDebugStoreArea(DebugStoreArea);
        _enable();
        KeSetAffinityThreadObj(Thread, ActivedProcessors);

        AllayedProcessor |= (KAFFINITY)1 << i;
        g_DSAreaArr[i] = DebugStoreArea;
        g_BtsBufferArr[i] = BtsBuffer;
        continue;

    _label_clear:
        if (DebugStoreArea) { ExFreePool(DebugStoreArea); }
        if (BtsBuffer) { ExFreePool(BtsBuffer); }
        Flag = FALSE;
        break;
    }
    ObDereferenceObject(Thread);



    if (Flag) {
        DbgPrint("SetupBTS Completed!\n");

        // Hook SwapContext
        PVOID KernelBase = GetKernelBase(NULL);
        PVOID SourceCode = (PUCHAR)KernelBase + 0x153af0;
        INTERCEPT_CREATE CreateInfo = { 0 };
        CreateInfo.patchType = PUSH_RET_14;
        InsertIntercept64(SourceCode, SwapContextHandler, &CreateInfo, &InterceptInfo);
        DbgPrint("Hook SwapContext Success!\n");

        return STATUS_SUCCESS;
    }
    else {
        DbgPrint("SetupBTS Failed!\n");
        ResetBTS();
        return STATUS_UNSUCCESSFUL;
    }


}
VOID ResetBTS() {
    PKTHREAD Thread = KeGetCurrentThread();
    ObReferenceObjectByPointer(Thread, 0, *PsThreadType, KernelMode);
    for (size_t i = 0; i < ProcessorCount; i++)
    {

        KAFFINITY CurrentAffinity = (KAFFINITY)1 << i;
        if ((AllayedProcessor & CurrentAffinity) == 0) {
            continue;
        }

        // 切换CPU
        KeSetAffinityThreadObj(Thread, CurrentAffinity);
        _disable();
        DisableBTS();
        SetDebugStoreArea(0);
        _enable();
        KeSetAffinityThreadObj(Thread, ActivedProcessors);

        AllayedProcessor &= ~CurrentAffinity;
        // 释放内存
        if (g_DSAreaArr[i]) {
            ExFreePool(g_DSAreaArr[i]);
            g_DSAreaArr[i] = NULL;
        }
        if (g_BtsBufferArr[i]) {
            ExFreePool(g_BtsBufferArr[i]);
            g_BtsBufferArr[i] = NULL;
        }

    }
    ObDereferenceObject(Thread);

    // Unhook
    RemoveIntercept(&InterceptInfo);
}



