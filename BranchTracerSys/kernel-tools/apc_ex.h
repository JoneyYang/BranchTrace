#pragma once
#include "nt_include.h"

EXTERN_C_START

/*
  Description: ʹ��APC�ķ�ʽCopyĿ������ڴ�
  ReturnValue: NTSTATUS
  Parameters:
    Process: Ŀ�����EPROCESSָ��
    Destination: Ŀ��ָ��
    Source: Դָ��
    Length: Size
*/
NTSTATUS ApcCopyProcessMemory(PEPROCESS Process, PVOID Destination, PVOID Source, SIZE_T BufferSize);

/* ����һ��NormalApc*/
NTSTATUS ApcInsertNormalApc(PEPROCESS Process, PVOID NormalRoutine, PVOID NormalContext, PVOID SystemArgument1, PVOID SystemArgument2);


EXTERN_C_END
