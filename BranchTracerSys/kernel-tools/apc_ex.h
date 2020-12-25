#pragma once
#include "nt_include.h"

EXTERN_C_START

/*
  Description: 使用APC的方式Copy目标进程内存
  ReturnValue: NTSTATUS
  Parameters:
    Process: 目标进程EPROCESS指针
    Destination: 目标指针
    Source: 源指针
    Length: Size
*/
NTSTATUS ApcCopyProcessMemory(PEPROCESS Process, PVOID Destination, PVOID Source, SIZE_T BufferSize);

/* 插入一个NormalApc*/
NTSTATUS ApcInsertNormalApc(PEPROCESS Process, PVOID NormalRoutine, PVOID NormalContext, PVOID SystemArgument1, PVOID SystemArgument2);


EXTERN_C_END
