#include "apc_ex.h"
#include "nt_imports.h"
#include "my_ntstatus.h"

#include <intrin.h>


typedef struct _APCEX_COPYMEM_INFO
{
    PVOID Destination;
    PVOID Source;
    SIZE_T Length;
    NTSTATUS Status;
} APCEX_COPYMEM_INFO, * PAPCEX_COPYMEM_INFO;

VOID CopyProcessMemoryApc(PKAPC Apc, PVOID* NormalRoutine, PVOID* NormalContext, PVOID* SystemArgument1, PVOID* SystemArgument2)
{
    UNREFERENCED_PARAMETER(NormalRoutine);
    UNREFERENCED_PARAMETER(Apc);
    UNREFERENCED_PARAMETER(NormalContext);
    PAPCEX_COPYMEM_INFO Info = (PAPCEX_COPYMEM_INFO)*SystemArgument1;
    PKEVENT Event = (PKEVENT)*SystemArgument2;

    //DbgPrint("Destination %p\n", Info->Destination);
    //DbgPrint("Source %p\n", Info->Source);
    //DbgPrint("Length %p\n", Info->Length);

    //PEPROCESS process = PsGetCurrentProcess();
    //PCHAR processName = PsGetProcessImageFileName(process);
    //DbgPrint("process: %s\n", processName);

    _disable();
    __writecr0(__readcr0() & ~0x10000);

    RtlCopyMemory(Info->Destination, Info->Source, Info->Length);

    __writecr0(__readcr0() | 0x10000);
    _enable();

    Info->Status = STATUS_SUCCESS;
    KeSetEvent(Event, IO_NO_INCREMENT, FALSE);
}


PETHREAD FindAnThread(PEPROCESS Process)
{
    // 遍历全局句柄表. 寻找一个目标进程的ETHREAD
    PETHREAD Thread = NULL;
    for (ULONG_PTR i = 8; i < 0x40000; i += 4) {
        PETHREAD t;

        if (NT_SUCCESS(PsLookupThreadByThreadId((HANDLE)i, &t))) {
            if (PsGetThreadProcess(t) == Process) {
                Thread = t;
                break;
            }
            ObDereferenceObject(t);
        }
    }
    return Thread;
}

NTSTATUS ApcCopyProcessMemory(PEPROCESS Process, PVOID Destination, PVOID Source, SIZE_T BufferSize)
{
    NTSTATUS Status = STATUS_SUCCESS;

    PETHREAD Thread = FindAnThread(Process);
    if (Thread == NULL) {
        return STATUS_THREAD_NOT_FOUND;
    }

    PKAPC Apc = (PKAPC)ExAllocatePoolWithTag(NonPagedPool, sizeof(KAPC), 'CPA.');
    PAPCEX_COPYMEM_INFO Info = (PAPCEX_COPYMEM_INFO)ExAllocatePoolWithTag(NonPagedPool, sizeof(APCEX_COPYMEM_INFO), 'CPA.');
    PKEVENT Event = (PKEVENT)ExAllocatePoolWithTag(NonPagedPool, sizeof(KEVENT), 'CPA.');
    if (!Apc || !Event || !Info) {
        Status = STATUS_MEMORY_ALLOCATE_UNSUCCESS;
        goto Clear;
    }

    Info->Destination = Destination;
    Info->Source = Source;
    Info->Length = BufferSize;

    KeInitializeEvent(Event, NotificationEvent, TRUE);
    KeClearEvent(Event);

    KeInitializeApc(Apc, Thread, OriginalApcEnvironment, (PVOID)CopyProcessMemoryApc, NULL, NULL, KernelMode, NULL);

    if (!KeInsertQueueApc(Apc, Info, Event, IO_NO_INCREMENT)) {
        Status = STATUS_APC_INSERT_FAILED;
        goto Clear;
    }

    KeWaitForSingleObject(Event, Executive, KernelMode, FALSE, NULL);
    Status = Info->Status;

Clear:
    if (Apc) {
        ExFreePool(Apc);
    }
    if (Event) {
        ExFreePool(Event);
    }
    if (Info) {
        ExFreePool(Info);
    }

    ObDereferenceObject(Thread);
    return Status;
}

void NTAPI KernelRoutine(PKAPC apc, PVOID* NormalRoutine, PVOID* NormalContext, PVOID* SystemArgument1, PVOID* SystemArgument2)
{
    //ExFreePool(apc);
    DbgPrint("KernelRoutine called!\n");
    DbgPrint("NormalRoutine %p! Value: %p\n", NormalRoutine, *(PULONG_PTR)NormalRoutine);
    DbgPrint("NormalContext %p! Value: %p\n", NormalContext, *(PULONG_PTR)NormalContext);
    DbgPrint("SystemArgument1 %p  Value: %p!\n", SystemArgument1, *(PULONG_PTR)SystemArgument1);
    DbgPrint("SystemArgument2 %p  Value: %p!\n", SystemArgument2, *(PULONG_PTR)SystemArgument2);

    //KeTestAlertThread(UserMode);
    ExFreePool(apc);
}

VOID RundownRoutine(IN struct _KAPC* Apc)
{
    UNREFERENCED_PARAMETER(Apc);
    DbgPrint("RundownRoutine called!");
}

NTSTATUS ApcInsertNormalApc(PEPROCESS Process, PVOID NormalRoutine, PVOID NormalContext, PVOID SystemArgument1, PVOID SystemArgument2)
{
    NTSTATUS Status = STATUS_SUCCESS;
    PETHREAD Thread = FindAnThread(Process);
    if (Thread == NULL) {
        return STATUS_THREAD_NOT_FOUND;
    }
    PKAPC Apc = (PKAPC)ExAllocatePoolWithTag(NonPagedPool, sizeof(KAPC), 'CPA.');
    if (!Apc) {
        Status = STATUS_MEMORY_ALLOCATE_UNSUCCESS;
        return Status;
    }

    //在64位系统中插入32位用户apc时apc函数地址需要求补后左移两位
    //NormalRoutine = (~(ULONG_PTR)NormalRoutine + 1) << 2;
    NormalRoutine = (PVOID)(((~(ULONG_PTR)NormalRoutine) + 1) << 2);


    KeInitializeApc(Apc, Thread, OriginalApcEnvironment, (PVOID)KernelRoutine, NULL, NormalRoutine, UserMode, NormalContext);

    //0x07a
    PKAPC_STATE ApcState = (PKAPC_STATE)((PUCHAR)Thread + 0x50);
    ApcState->UserApcPending = TRUE;

    if (!KeInsertQueueApc(Apc, SystemArgument1, SystemArgument2, IO_NO_INCREMENT))
    {
        return STATUS_APC_INSERT_FAILED
    }

    return STATUS_SUCCESS;
}
