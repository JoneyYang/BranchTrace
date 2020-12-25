#pragma once
#include <windows.h>
#include "..\BranchTracerSys\ioctls.h"
#include <vector>
#include "pluginsdk\_scriptapi_flag.h"
//#include "LDasm/LDasm.h"

#include "LDasm/LDasm.h"

typedef struct _BTS_RECORD64
{
    ULONG64 from;   // 记录分支的源地址
    ULONG64 to;     // 记录分支的目标地址
    union
    {
        ULONG64 branchPredicted;
        ULONG64 branchType;
    };
}BTS_RECORD64, * PBTS_RECORD64;



enum BRANCH_TYPE
{
    BRANCH_TYPE_UNKOWN,
    BRANCH_TYPE_RET,
    BRANCH_TYPE_JCC,
    BRANCH_TYPE_CALL
};
bool StartTrace(DWORD ThreadId);
bool StopTrace();
bool StopAndFetchTrace(DWORD ThreadId, ULONG64 filterStart, ULONG64 fileterEnd, DWORD Records, PBTS_RECORD64* lpRecordStart, PBTS_RECORD64* lpRecordEnd);



class TraceNode {

public:
    // pRecord.from
    ULONG_PTR from;
    // pRecord.to
    ULONG_PTR to;
    DWORD branchType;

    bool hasPrev;
    bool hasNext;
    int prevIndex;    // 跳过中间的函数调用
    int nextIndex;
    int procStartIndex;
    std::vector<int> procSubIndex;


    // pRecord.to ~ nextRecord.from
    std::vector<ULONG_PTR> paths;


};

class Proc
{
    std::vector<TraceNode> nodes;
};


class TraceCursor {
public:
    int nodeIndex;
    int pathIndex;

    std::vector<TraceNode> nodes;
    TraceNode& currentNode() {
        return nodes[nodeIndex];
    }

    ULONG_PTR currentAddr() {
        return nodes[nodeIndex].paths[pathIndex];
    }
    DWORD branchType() {
        return nodes[nodeIndex].branchType;
    }
    ULONG_PTR currentFrom() {
        return nodes[nodeIndex].from;
    }
    ULONG_PTR currentTo() {
        return nodes[nodeIndex].to;
    }


    bool isAtTo();
    bool isAtFrom();

    bool toEndInstr();
    bool toPrevInstrInto();
    bool toNextInstrInto();
    bool toPrevInstr();
    bool toNextInstr();
    bool toProcStart();
    bool toProcEnd();
    bool toProcPrevNode();
    bool toProcNextNode();
};


