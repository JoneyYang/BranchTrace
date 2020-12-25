#pragma once
#include "nt_include.h"
#include "nt_enums.h"
#include "nt_structs.h"

EXTERN_C_START

NTKERNELAPI VOID KeInitializeApc(__out PRKAPC Apc, __in PETHREAD Thread, __in KAPC_ENVIRONMENT Environment, __in PVOID KernelRoutine,
                                 __in_opt PVOID RundownRoutine, __in_opt PVOID NormalRoutine, __in_opt KPROCESSOR_MODE ProcessorMode, __in_opt PVOID NormalContext);
NTSYSAPI BOOLEAN KeInsertQueueApc(__inout PRKAPC Apc, __in_opt PVOID SystemArgument1, __in_opt PVOID SystemArgument2, __in KPRIORITY Increment);
NTSYSAPI BOOLEAN KeTestAlertThread(__in KPROCESSOR_MODE AlertMode);

NTSYSAPI PPEB PsGetProcessPeb(IN PEPROCESS Process);
NTSYSAPI PCHAR PsGetProcessImageFileName(PEPROCESS Process);

NTSYSAPI NTSTATUS IoCreateDriver(IN PUNICODE_STRING DriverName, OPTIONAL IN PDRIVER_INITIALIZE InitializationFunction);
NTSYSAPI NTSTATUS NTAPI MmCopyVirtualMemory(PEPROCESS SourceProcess, PVOID SourceAddress, PEPROCESS TargetProcess, PVOID TargetAddress, SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode, PSIZE_T ReturnSize);
NTSYSAPI NTSTATUS NTAPI ZwProtectVirtualMemory(__in HANDLE ProcessHandle, __inout PVOID* BaseAddress, __inout PSIZE_T RegionSize, __in ULONG NewProtect, __out PULONG OldProtect);
NTSYSAPI NTSTATUS NTAPI ZwQuerySystemInformation(IN SYSTEM_INFORMATION_CLASS SystemInformationClass, OUT PVOID SystemInformation, IN ULONG SystemInformationLength, OUT PULONG ReturnLength OPTIONAL);

EXTERN_C_END
