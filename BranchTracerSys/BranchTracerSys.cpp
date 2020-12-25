#include <ntifs.h>
#include <ntddk.h>
#include "asm/debug.h"
#include "bts/bts.h"
#include "kernel-hooks/Hooks.h"
#include "kernel-tools/KernelBase.h"

#include "ioctls.h"

NTSTATUS IoctlDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS IoctlCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);

PDEVICE_OBJECT DeviceObject;
UNICODE_STRING DeviceName;
UNICODE_STRING SymlinkName;

VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);
    DbgPrint("Driver Unloading... \n");

    IoDeleteSymbolicLink(&SymlinkName);
    IoDeleteDevice(DriverObject->DeviceObject);

    // Reset BTS : 释放DS_Area内存. 
    ResetBTS();

    // 监控期间会不断产生TraceData. 结束时必须释放这些内存。
    ClearThreadTraceData();
}

//StartMonitThread((HANDLE)THREADID);
//StopMonitThread((HANDLE)THREADID);


#define  DEVICE_NAME L"\\Device\\Branch-Trace"
#define  SYMLINK_NAME L"\\DosDevices\\Branch-Trace"
#define  DRIVER_NAME L"\\Driver\\Branch-Trace"

EXTERN_C NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING pRegistryPath)
{
    UNREFERENCED_PARAMETER(pRegistryPath);

    // 初始化环境 : 初始化全局变量
    InitializeEnvironment();

    // Setup 
    if (!NT_SUCCESS(SetupBTS())) {
        return STATUS_UNSUCCESSFUL;
    }


    RtlInitUnicodeString(&DeviceName, DEVICE_NAME);
    RtlInitUnicodeString(&SymlinkName, SYMLINK_NAME);


    // 创建设备对象
    IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);

    // 设置通讯方式
    DeviceObject->Flags |= DO_BUFFERED_IO;

    IoCreateSymbolicLink(&SymlinkName, &DeviceName);


    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoctlDeviceControl;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = IoctlCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = IoctlCreateClose;
    DriverObject->DriverUnload = DriverUnload;

    DbgPrint("DriverEntry Completed!\n");
    return STATUS_SUCCESS;
}


NTSTATUS IoctlCreateClose(PDEVICE_OBJECT pDeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(pDeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

NTSTATUS IoctlDeviceControl(PDEVICE_OBJECT pDeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(pDeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    NTSTATUS Status = STATUS_SUCCESS;
    ULONG InfoSize = 0;
    PIO_STACK_LOCATION Stack = IoGetCurrentIrpStackLocation(Irp);
    ULONG ControlCode = Stack->Parameters.DeviceIoControl.IoControlCode;


    switch (ControlCode) {
        case IOCTL_START_THREAD_TRACE: {
            // 开始监控
            PSTART_THREAD_TRACE_PARAM Request = (PSTART_THREAD_TRACE_PARAM)Irp->AssociatedIrp.SystemBuffer;
            if (Request->ThreadId == 0) {
                Status = STATUS_UNSUCCESSFUL;
                break;
            }

            if (!StartMonitThread((HANDLE)Request->ThreadId)) {
                DbgPrint("StartMonitThread Failed!\n");
                Status = STATUS_UNSUCCESSFUL;
                break;
            }
            break;
        }

        case IOCTL_STOP_THREAD_TRACE:
        {
            PSTOP_THREAD_TRACE_PARAM Request = (PSTOP_THREAD_TRACE_PARAM)Irp->AssociatedIrp.SystemBuffer;
            StopMonitThread((HANDLE)Request->ThreadId);

            DbgBreakPoint();
            SIZE_T BufferSize = 0;
            if (Request->Buffer != NULL) {
                if (!ReadThreadTraceData((HANDLE)Request->ThreadId, Request->Buffer,
                    Request->BufferSize, Request->FilterStart, Request->FilterEnd, &BufferSize)) {
                    Status = STATUS_UNSUCCESSFUL;
                    break;
                }
            }

            // 清理数据
            ClearThreadTraceData();
            Request->ReadSize = BufferSize;
            InfoSize = sizeof(STOP_THREAD_TRACE_PARAM);
            Status = STATUS_SUCCESS;
            break;

        }


        default: {
            Status = STATUS_INVALID_DEVICE_REQUEST;
            InfoSize = 0;
            break;
        }
    }

    Irp->IoStatus.Status = Status;
    Irp->IoStatus.Information = InfoSize;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return Status;
}

