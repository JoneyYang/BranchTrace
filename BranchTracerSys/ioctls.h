#pragma once

#ifdef _WDMDDK_
#include <ntddk.h>
#else
#include <windows.h>
#endif

#define IOCTL_START_THREAD_TRACE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x808, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_STOP_THREAD_TRACE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x809, METHOD_BUFFERED, FILE_ANY_ACCESS)


typedef struct _START_THREAD_TRACE_PARAM {
    union
    {
        ULONG_PTR ThreadId;
        ULONG64 ullThreadId;
    };
}START_THREAD_TRACE_PARAM, * PSTART_THREAD_TRACE_PARAM;

typedef struct _STOP_THREAD_TRACE_PARAM {
    union
    {
        ULONG_PTR ThreadId;
        ULONG64 ullThreadId;
    };

    union {
        ULONG_PTR FilterStart;
        ULONG64 ullFilterStart;
    };

    union {
        ULONG_PTR FilterEnd;
        ULONG64 ullFilterStop;
    };

    union {
        SIZE_T ReadSize;
        ULONG64 ullReadSize;
    };

    union {
        PVOID Buffer;
        ULONG64 ullBuffer;
    };
    union {
        SIZE_T BufferSize;
        ULONG64 ullBufferSize;
    };


}STOP_THREAD_TRACE_PARAM, * PSTOP_THREAD_TRACE_PARAM;
