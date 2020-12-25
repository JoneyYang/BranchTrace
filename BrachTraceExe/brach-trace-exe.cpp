#include <windows.h>
#include "..\BranchTracerSys\ioctls.h"
#include <stdio.h>



typedef struct _BTS_RECORD64
{
    ULONG64 from;   // 记录分支的源地址
    ULONG64 to;     // 记录分支的目标地址
    ULONG64 branchPredicted;
}BTS_RECORD64, * PBTS_RECORD64;



int main() {
    HANDLE hDevice = CreateFileA("\\\\.\\Branch-Trace", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    ULONG ThreadId = 0;
    printf("Enter thread id:");
    scanf_s("%d", &ThreadId);

    START_THREAD_TRACE_PARAM StartRequest = { 0 };
    StartRequest.ThreadId = ThreadId;

    DWORD BytesReturned = 0;
    if (!DeviceIoControl(hDevice, IOCTL_START_THREAD_TRACE,
        (LPVOID)&StartRequest, sizeof(StartRequest),
        (LPVOID)&StartRequest, sizeof(StartRequest),
        &BytesReturned, NULL))
    {
        printf("Start Trace failed!\n");
        return -1;
    }

    printf("Press Enter to conttinue\n");
    system("pause");

#define BIFFER_SIZE 0x2000
    PVOID Buffer = VirtualAlloc(NULL, BIFFER_SIZE, MEM_COMMIT, PAGE_READWRITE);
    STOP_THREAD_TRACE_PARAM StopRequest = { 0 };
    StopRequest.ThreadId = ThreadId;
    StopRequest.Buffer = Buffer;
    StopRequest.BufferSize = BIFFER_SIZE;
    StopRequest.FilterStart = 0x00400000;
    StopRequest.FilterEnd = 0x00D4C000+0x4000;


    if (!DeviceIoControl(hDevice, IOCTL_STOP_THREAD_TRACE,
        (LPVOID)&StopRequest, sizeof(StopRequest),
        (LPVOID)&StopRequest, sizeof(StopRequest),
        &BytesReturned, NULL))
    {
        printf("Stop Trace failed!\n");
        getchar();
        return -1;
    }

    printf("Successful Buffer:%p ReadSize:%x\n", StopRequest.Buffer, StopRequest.ReadSize);


    PBTS_RECORD64 RecordStart = (PBTS_RECORD64)Buffer;
    PBTS_RECORD64 RecordEnd = (PBTS_RECORD64)((ULONG_PTR)Buffer + StopRequest.ReadSize);


    // 逆排, 从驱动拉回来的记录顺序跟执行顺序的相反.
    for (PBTS_RECORD64 p1 = RecordStart, p2 = RecordEnd - 1;
        p1 < p2;
        p1++, p2--)
    {
        BTS_RECORD64 temp = *p1;
        *p1 = *p2;
        *p2 = temp;
    }

    PBTS_RECORD64 pRecord = RecordStart;
    while (pRecord < RecordEnd)
    {
        printf("%llx-> %llx\n", pRecord->from, pRecord->to);
        pRecord++;
    }

    system("pause");
    getchar();


}