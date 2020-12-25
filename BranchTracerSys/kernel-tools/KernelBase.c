#include "KernelBase.h"

#include "nt_imports.h"

PVOID g_KernelBase = NULL;
ULONG g_KernelSize = 0;

PVOID GetKernelBase(OUT PULONG ImageSize) {
    NTSTATUS Status;
    if (g_KernelBase != NULL) {
        if (ImageSize) {
            *ImageSize = g_KernelSize;
        }
        return g_KernelBase;
    }

    UNICODE_STRING RoutineName = RTL_CONSTANT_STRING(L"NtCreateFile");
    PVOID AnchorPointer = MmGetSystemRoutineAddress(&RoutineName);
    if (AnchorPointer == NULL) {
        return NULL;
    }

    ULONG InfoSize = 0;
    ZwQuerySystemInformation(SystemModuleInformation, NULL, 0, &InfoSize);
    if (InfoSize == 0) {
        return NULL;
    }
    PRTL_PROCESS_MODULES ModuleInfo =
        (PRTL_PROCESS_MODULES)ExAllocatePoolWithTag(PagedPool, InfoSize, 'sabK');
    RtlZeroMemory(ModuleInfo, InfoSize);

    Status = ZwQuerySystemInformation(SystemModuleInformation, ModuleInfo, InfoSize, &InfoSize);
    if (NT_SUCCESS(Status)) {
        PRTL_PROCESS_MODULE_INFORMATION Module = ModuleInfo->Modules;

        for (size_t i = 0; i < ModuleInfo->NumberOfModules; i++) {
            ULONG ModuleImageSize = Module[i].ImageSize;
            PVOID ModuleImageBase = Module[i].ImageBase;

            if (AnchorPointer >= ModuleImageBase && (ULONG_PTR)AnchorPointer <= ((ULONG_PTR)ModuleImageBase + ModuleImageSize)) {
                g_KernelBase = ModuleImageBase;
                g_KernelSize = ModuleImageSize;

                if (ImageSize) {
                    *ImageSize = ModuleImageSize;
                }
                break;
            }
        }
    }

    if (ModuleInfo) {
        ExFreePool(ModuleInfo);
    }

    return g_KernelBase;
}
