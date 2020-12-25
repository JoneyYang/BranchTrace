#include "ThreadTraceCore.h"
#include <vector>



bool StartTrace(DWORD ThreadId)
{

    HANDLE hDevice = CreateFileA("\\\\.\\Branch-Trace", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    START_THREAD_TRACE_PARAM StartRequest = { 0 };
    StartRequest.ThreadId = ThreadId;

    DWORD BytesReturned = 0;
    if (!DeviceIoControl(hDevice, IOCTL_START_THREAD_TRACE,
        (LPVOID)&StartRequest, sizeof(StartRequest),
        (LPVOID)&StartRequest, sizeof(StartRequest),
        &BytesReturned, NULL))
    {
        return false;
    }

    return true;
}

bool StopTrace()
{
    HANDLE hDevice = CreateFileA("\\\\.\\Branch-Trace", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hDevice == INVALID_HANDLE_VALUE) {
        return false;
    }
    DWORD BytesReturned = 0;
    STOP_THREAD_TRACE_PARAM StopRequest = { 0 };
    StopRequest.ThreadId = NULL;
    StopRequest.Buffer = NULL;
    StopRequest.BufferSize = 0;
    StopRequest.FilterStart = 0;
    StopRequest.FilterEnd = 0;

    if (!DeviceIoControl(hDevice, IOCTL_STOP_THREAD_TRACE,
        (LPVOID)&StopRequest, sizeof(StopRequest),
        (LPVOID)&StopRequest, sizeof(StopRequest),
        &BytesReturned, NULL))
    {
        CloseHandle(hDevice);
        return false;
    }

    CloseHandle(hDevice);
    return true;
}

bool StopAndFetchTrace(DWORD ThreadId, ULONG64 filterStart, ULONG64 fileterEnd, DWORD Records, PBTS_RECORD64* lpRecordStart, PBTS_RECORD64* lpRecordEnd)
{
    if (Records <= 0 || lpRecordStart == NULL || lpRecordEnd == NULL) {
        return false;
    }

    HANDLE hDevice = CreateFileA("\\\\.\\Branch-Trace", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (hDevice == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD bufferSize = Records * sizeof(BTS_RECORD64);
    PVOID Buffer = VirtualAlloc(NULL, bufferSize, MEM_COMMIT, PAGE_READWRITE);
    if (Buffer == NULL) {
        CloseHandle(hDevice);
        return false;
    }

    //STOP_THREAD_TRACE_PARAM StopRequest = { 0 };
    //StopRequest.ThreadId = ThreadId;
    //StopRequest.Buffer = Buffer;
    //StopRequest.BufferSize = bufferSize;
    //StopRequest.FilterStart = (ULONG64)filterStart;
    //StopRequest.FilterEnd = (ULONG64)fileterEnd;

    STOP_THREAD_TRACE_PARAM StopRequest = { 0 };
    StopRequest.ThreadId = ThreadId;
    StopRequest.Buffer = Buffer;
    StopRequest.BufferSize = 0x2000;
    StopRequest.FilterStart = 0x00400000;
    StopRequest.FilterEnd = 0x00D4C000 + 0x4000;

    DWORD BytesReturned = 0;
    if (!DeviceIoControl(hDevice, IOCTL_STOP_THREAD_TRACE,
        (LPVOID)&StopRequest, sizeof(StopRequest),
        (LPVOID)&StopRequest, sizeof(StopRequest),
        &BytesReturned, NULL))
    {
        VirtualFree(Buffer, 0, MEM_RELEASE);
        CloseHandle(hDevice);
        return false;
    }

    if (StopRequest.ReadSize == 0) {
        VirtualFree(Buffer, 0, MEM_RELEASE);
        CloseHandle(hDevice);
        return false;
    }


    PBTS_RECORD64 RecordStart = (PBTS_RECORD64)Buffer;
    PBTS_RECORD64 RecordEnd = (PBTS_RECORD64)((PUCHAR)Buffer + StopRequest.ReadSize);

    // ÄæÅÅ
    for (PBTS_RECORD64 p1 = RecordStart, p2 = RecordEnd - 1;
        p1 < p2;
        p1++, p2--)
    {
        BTS_RECORD64 temp = *p1;
        *p1 = *p2;
        *p2 = temp;
    }



    *lpRecordStart = RecordStart;
    *lpRecordEnd = RecordEnd;

    CloseHandle(hDevice);
    return true;
}

bool TraceCursor::isAtTo()
{
    return pathIndex == 1;
}

bool TraceCursor::isAtFrom()
{
    return pathIndex == 0;
}

bool TraceCursor::toEndInstr()
{
    nodeIndex = nodes.size() - 1;
    pathIndex = currentNode().paths.size() - 1;
    return true;
}

bool TraceCursor::toPrevInstrInto()
{
    if (pathIndex > 0) {
        pathIndex--;
        return true;
    }

    if (nodeIndex > 0) {
        nodeIndex--;
        pathIndex = currentNode().paths.size() - 1;
        return true;
    }

    return false;
}

bool TraceCursor::toNextInstrInto()
{
    if (pathIndex < currentNode().paths.size() - 1) {
        pathIndex++;
        return true;
    }


    //int pathSize = currentNode().paths.size();
    //if (pathSize > 0 && pathIndex < pathSize - 1) {
    //    pathIndex++;
    //    return true;
    //}

    if (nodeIndex < nodes.size() - 1) {
        nodeIndex++;
        pathIndex = 0;
        return true;
    }

    return false;

    //int nodeSize = nodes.size();
    //if (nodeSize > 0 && nodeIndex < nodeSize - 1)
    //{
    //    nodeIndex++;
    //    pathIndex = 0;
    //    return true;
    //}

    //return false;
}

bool TraceCursor::toPrevInstr()
{
    if (pathIndex > 0) {
        pathIndex--;
        return true;
    }

    if (currentNode().hasPrev) {
        nodeIndex = currentNode().prevIndex;
        pathIndex = currentNode().paths.size() - 1;
        return true;
    }

    //if (nodeIndex > 0)
    //{
    //    nodeIndex--;
    //    pathIndex = currentNode().paths.size() - 1;
    //    return true;
    //}

    return false;
}

bool TraceCursor::toNextInstr()
{
    if (pathIndex < currentNode().paths.size() - 1) {
        pathIndex++;
        return true;
    }

    if (currentNode().hasNext) {
        nodeIndex = currentNode().nextIndex;
        pathIndex = 0;
        return true;
    }

    return false;
}

bool TraceCursor::toProcStart()
{
    nodeIndex = currentNode().procStartIndex;
    pathIndex = 0;
    return true;
}

bool TraceCursor::toProcEnd()
{
    if (currentNode().procSubIndex.size() > 0) {
        nodeIndex = currentNode().procSubIndex.back();
        pathIndex = currentNode().paths.size() - 1;
        return true;
    }

    return false;
}

bool TraceCursor::toProcPrevNode()
{
    if (currentNode().hasPrev) {
        nodeIndex = currentNode().prevIndex;
    }
    pathIndex = 0;
    return true;
}

bool TraceCursor::toProcNextNode()
{
    if (currentNode().hasNext) {
        nodeIndex = currentNode().nextIndex;
    }
    pathIndex = 0;
    return true;
}

