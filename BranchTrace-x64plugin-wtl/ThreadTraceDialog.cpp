#include "ThreadTraceDialog.h"
#include "pluginsdk/bridgemain.h"
#include "pluginsdk/_scriptapi_memory.h"
#include "disassembler.h"
#include "pluginsdk/_scriptapi_gui.h"
#include "plugin_global.h"
#include "pluginsdk/_scriptapi_misc.h"
#include <stack>
#include <TlHelp32.h>

ThreadTraceDialog::~ThreadTraceDialog()
{

}

BOOL ThreadTraceDialog::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
    DoDataExchange(FALSE);
    return TRUE;
}


void ThreadTraceDialog::OnCloseDialog()
{
    ShowWindow(SW_HIDE);
    ResetAll();
}

LRESULT ThreadTraceDialog::OnDropdownComboboxThread(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // TODO: Add your control notification handler code here
    THREADLIST threadList;
    DbgGetThreadList(&threadList);

    m_comboThread.ResetContent();

    // 遍历所有线程初始化列表
    for (size_t i = 0; i < threadList.count; i++)
    {
        THREADALLINFO* thread = &threadList.list[i];

        bool isMainThread = thread->BasicInfo.ThreadNumber == 0;
        bool isCurrentThead = i == threadList.CurrentThread;

        CString str;
        str.Format("[%d] thread-%x %s %s", i, thread->BasicInfo.ThreadId, isMainThread ? "[main]" : "", isCurrentThead ? "[current]" : "");
        m_comboThread.InsertString(i, str);
        m_comboThread.SetItemData(i, thread->BasicInfo.ThreadId);

        if (isMainThread) {
            m_comboThread.SetCurSel(i);
        }

    }
    return 1;
}

LRESULT ThreadTraceDialog::OnDropdownComboboxModule(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    HANDLE processHandle = DbgGetProcessHandle();
    if (processHandle == NULL) {
        return true;
    }

    HANDLE Snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, DbgGetProcessId());
    if (Snapshot == INVALID_HANDLE_VALUE)
    {
        return true;
    }

    CString str;
    str = "mod.main()";
    duint mainModule = 0;
    Script::Misc::ParseExpression(str.GetBuffer(), &mainModule);

    m_comboModule.ResetContent();
    MODULEENTRY32 ModuleInfo = { sizeof(MODULEENTRY32) };
    if (Module32First(Snapshot, &ModuleInfo))
    {
        do {

            duint moduleSize = 0;
            str.Format("mod.size(%p)", ModuleInfo.hModule);
            Script::Misc::ParseExpression(str.GetBuffer(), &moduleSize);

            duint moduleEnd = (duint)ModuleInfo.hModule + moduleSize;

            bool isMainModule = mainModule == (duint)ModuleInfo.hModule;
            duint isUserModule = false;
            duint isSystemModule = false;
            str.Format("mod.user(%p)", ModuleInfo.hModule);
            Script::Misc::ParseExpression(str.GetBuffer(), &isUserModule);
            str.Format("mod.system(%p)", ModuleInfo.hModule);
            Script::Misc::ParseExpression(str.GetBuffer(), &isSystemModule);


            str.Format("[%-4s][%p] %s\n",
                isMainModule ? "main" : isUserModule ? "user" : isSystemModule ? "syst" : "",
                ModuleInfo.hModule, ModuleInfo.szModule);
            int index = m_comboModule.InsertString(m_comboModule.GetCount(), str);
            m_comboModule.SetItemData(index, (ULONG_PTR)ModuleInfo.hModule);

            if (isMainModule) {
                m_comboModule.SetCurSel(index);
            }

        } while (Module32Next(Snapshot, &ModuleInfo));
    }
    return true;
}

LRESULT ThreadTraceDialog::OnBnClickedButtonStarttrace(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    // 开启跟踪
    int cursel = m_comboThread.GetCurSel();
    if (cursel == -1) {
        MessageBox(TEXT("You must select a thread!"));
        return TRUE;
    }

    cursel = m_comboModule.GetCurSel();
    if (cursel == -1) {
        MessageBox(TEXT("You must select a module!"));
        return TRUE;
    }


#ifdef DEBUG

#else
    if (!StartTrace(m_comboThread.GetItemData(cursel))) {
        MessageBox(TEXT("Start Trace failed!"));
        return FALSE;
    }
#endif // DEBUG

    m_bTracingFlag = TRUE;

    // 调整界面
    m_buttonStarttrace.EnableWindow(FALSE);
    m_buttonReset.EnableWindow(TRUE);
    m_comboThread.EnableWindow(FALSE);
    m_comboModule.EnableWindow(FALSE);

    return TRUE;
}

LRESULT ThreadTraceDialog::OnBnClickedButtonReset(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    ResetAll();
    return TRUE;
}



LRESULT ThreadTraceDialog::OnBnClickedButtonPrevInto(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    cursor.toPrevInstrInto();
    ULONG_PTR addr = cursor.currentAddr();
    GuiDisasmAt(addr, addr);
    return TRUE;
}


LRESULT ThreadTraceDialog::OnBnClickedButtonNextInto(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    cursor.toNextInstrInto();
    ULONG_PTR addr = cursor.currentAddr();
    GuiDisasmAt(addr, addr);
    return TRUE;
}

LRESULT ThreadTraceDialog::OnBnClickedButtonPrev(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

    // 获取上一条指令的地址.
    cursor.toPrevInstr();
    GuiDisasmAt(cursor.currentAddr(), cursor.currentAddr());

    return TRUE;
}


LRESULT ThreadTraceDialog::OnBnClickedButtonNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

    // 获取下一条指令的地址.
    cursor.toNextInstr();
    GuiDisasmAt(cursor.currentAddr(), cursor.currentAddr());

    return TRUE;
}

LRESULT ThreadTraceDialog::OnBnClickedButtonProcStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    cursor.toProcStart();
    GuiDisasmAt(cursor.currentAddr(), cursor.currentAddr());


    CString str;
    TraceNode node = cursor.currentNode();


    for (size_t j = 0; j < node.paths.size(); j++)
    {
        str.Format("traceexecute %p", node.paths[j]);
        DbgCmdExec(str.GetBuffer());
    }

    while (node.hasNext)
    {
        node = cursor.nodes[node.nextIndex];
        for (size_t j = 0; j < node.paths.size(); j++)
        {
            str.Format("traceexecute %p", node.paths[j]);
            DbgCmdExec(str.GetBuffer());
        }
    }

    return TRUE;
}

LRESULT ThreadTraceDialog::OnBnClickedButtonProcEnd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    cursor.toProcEnd();
    GuiDisasmAt(cursor.currentAddr(), cursor.currentAddr());
    return TRUE;
}

LRESULT ThreadTraceDialog::OnBnClickedButtonPrevSnippet(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    cursor.toProcPrevNode();
    GuiDisasmAt(cursor.currentAddr(), cursor.currentAddr());
    return TRUE;
}

LRESULT ThreadTraceDialog::OnBnClickedButtonNextSnippet(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    cursor.toProcNextNode();
    GuiDisasmAt(cursor.currentAddr(), cursor.currentAddr());
    return TRUE;
}

LRESULT ThreadTraceDialog::OnBnClickedButtonToEnd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    cursor.toEndInstr();
    ULONG_PTR addr = cursor.currentAddr();
    GuiDisasmAt(addr, addr);

    return TRUE;
}

void ThreadTraceDialog::OnBreakDebugPoint()
{
    if (m_bTracingFlag != TRUE) {
        return;
    }

    int threadComboCursel = m_comboThread.GetCurSel();
    if (threadComboCursel == -1) {
        return;
    }

    int moduleComboCursel = m_comboModule.GetCurSel();
    if (moduleComboCursel == -1) {
        return;
    }


    // 追踪的线程ID
    DWORD_PTR traceThreadId = m_comboThread.GetItemData(threadComboCursel);
    DWORD_PTR moduleBase = m_comboModule.GetItemData(moduleComboCursel);
    CString temp;
    //duint moduleSize = 0;
    //temp.Format("mem.size(%p)", moduleBase);
    //Script::Misc::ParseExpression(temp.GetBuffer(), &moduleSize);
    duint moduleSize = 0;
    temp.Format("mod.size(%p)", moduleBase);
    Script::Misc::ParseExpression(temp.GetBuffer(), &moduleSize);
    DWORD_PTR moduleEnd = moduleBase + moduleSize;



    // 获取被中断的线程信息
    THREADLIST threadList;
    DbgGetThreadList(&threadList);
    DWORD breakedThread = threadList.list[threadList.CurrentThread].BasicInfo.ThreadId;

    if (traceThreadId != breakedThread) {
        return;
    }

    // 停止监控
    if (!StopAndFetchTrace(traceThreadId, moduleBase, moduleEnd, 5000, &RecordStart, &RecordEnd)) {
        MessageBox("Fetch trace record failed!");
        m_bTracingFlag = FALSE;
        return;
    }

    m_bTracingFlag = FALSE;


    // 解析指令
    Disassembler* disasm = PluginGlobal::GetInstance()->disassembler;
    Instruction instrction;
    duint readSize;
    unsigned char instrBuffer[0x10];
    PBTS_RECORD64 pRecord = RecordEnd - 1;
    while (pRecord >= RecordStart)
    {
        Script::Memory::Read((duint)pRecord->from, instrBuffer, 0x10, &readSize);

        if (!disasm->Disasm(instrBuffer, readSize, &instrction)) {
            _plugin_logprintf("BranchTrace Error: Disasm failed! codes:");
            for (duint i = 0; i < readSize; i++) {
                _plugin_logprintf("%x ", instrBuffer[i]);
            }
            continue;
        }
        ZydisMnemonic mnemonic = instrction.zydisInstrction.mnemonic;
        if (mnemonic == ZYDIS_MNEMONIC_RET) {
            pRecord->branchType = BRANCH_TYPE_RET;
        }
        else if (ZYDIS_MNEMONIC_JB <= mnemonic && mnemonic <= ZYDIS_MNEMONIC_JZ) {
            pRecord->branchType = BRANCH_TYPE_JCC;
        }
        else if (mnemonic == ZYDIS_MNEMONIC_CALL) {
            pRecord->branchType = BRANCH_TYPE_CALL;
        }
        else {
            // UNKOWN Instrction;
            pRecord->branchType = BRANCH_TYPE_UNKOWN;
            _plugin_logprintf("UNKOWN at : %p  mnemonic:%d \n", pRecord->from, instrction.zydisInstrction.mnemonic);
        }

        pRecord--;
    }



    // 存储数据
    cursor.nodes.clear();
    int instrCount = 0;

    std::map<ULONG64, int> PrcoToMap;;
    std::stack<int> ProcStak;

    CString str;
    pRecord = RecordStart;

    while (pRecord < RecordEnd)
    {
        int currentNodeIndex = cursor.nodes.size();
        //  加入列表
        cursor.nodes.emplace_back();
        TraceNode& node = cursor.nodes.back();

        node.from = pRecord->from;
        node.to = pRecord->to;
        node.branchType = pRecord->branchType;

        node.paths.push_back(pRecord->to);
        instrCount++;

        // 填充代码块
        PBTS_RECORD64 pNextRecord = pRecord + 1;

        str.Format("dis.next(%p)", pRecord->to);
        duint nextInstr = 0;
        Script::Misc::ParseExpression(str.GetBuffer(), &nextInstr);

        if (pNextRecord < RecordEnd)
        {
            // 避免脏数据,大于500字节的代码块直接跳过
            ULONG64 offset = pNextRecord->from - nextInstr;
            while (nextInstr != 0 && nextInstr <= pNextRecord->from && offset < 500)
            {
                node.paths.push_back(nextInstr);
                instrCount++;

                str.Format("dis.next(%p)", nextInstr);
                nextInstr = 0;
                Script::Misc::ParseExpression(str.GetBuffer(), &nextInstr);
            }

        }

        // 标记procStartIndex  procSubNodeIndex
        switch (node.branchType)
        {
            case BRANCH_TYPE_CALL:
                ProcStak.push(currentNodeIndex);
                node.procStartIndex = currentNodeIndex;
                cursor.nodes[currentNodeIndex].procSubIndex.push_back(currentNodeIndex);
                break;

            case BRANCH_TYPE_RET:

                if (!ProcStak.empty()) {
                    ProcStak.pop();
                }

                if (!ProcStak.empty()) {
                    node.procStartIndex = ProcStak.top();
                    cursor.nodes[ProcStak.top()].procSubIndex.push_back(currentNodeIndex);
                }
                else {
                    node.procStartIndex = currentNodeIndex;
                    cursor.nodes[currentNodeIndex].procSubIndex.push_back(currentNodeIndex);
                }
                break;

            case BRANCH_TYPE_JCC:
                if (!ProcStak.empty()) {
                    node.procStartIndex = ProcStak.top();
                    cursor.nodes[ProcStak.top()].procSubIndex.push_back(currentNodeIndex);

                }
                else {
                    node.procStartIndex = currentNodeIndex;
                    cursor.nodes[currentNodeIndex].procSubIndex.push_back(currentNodeIndex);
                }

                break;
            default:
                break;
        }

        // 标记 Prco链   1~2~3~4
        if (node.branchType == BRANCH_TYPE_CALL) {

            duint nextInstr = 0;
            str.Format("dis.next(%p)", pRecord->from);
            Script::Misc::ParseExpression(str.GetBuffer(), &nextInstr);
            if (nextInstr != 0) {
                PrcoToMap[nextInstr] = currentNodeIndex - 1;
            }

        }
        else if (node.branchType == BRANCH_TYPE_RET)
        {
            if (PrcoToMap.count(node.to) > 0) {
                auto preIndex = PrcoToMap[node.to];
                auto curIndex = currentNodeIndex;

                cursor.nodes[preIndex].nextIndex = curIndex;
                cursor.nodes[preIndex].hasNext = true;
                cursor.nodes[curIndex].prevIndex = preIndex;
                cursor.nodes[curIndex].hasPrev = true;

                PrcoToMap.erase(node.to);
            }
        }
        else if (node.branchType == BRANCH_TYPE_JCC)
        {
            if (currentNodeIndex > 0) {
                auto preIndex = currentNodeIndex - 1;
                auto curIndex = currentNodeIndex;
                cursor.nodes[preIndex].nextIndex = curIndex;
                cursor.nodes[preIndex].hasNext = true;
                cursor.nodes[curIndex].prevIndex = preIndex;
                cursor.nodes[curIndex].hasPrev = true;
            }
        }
        pRecord++;
    }





    // 设置起始位置
    cursor.toEndInstr();
    GuiDisasmAt(cursor.currentAddr(), cursor.currentAddr());

    // 设置控件状态
    m_comboThread.EnableWindow(FALSE);
    m_buttonPrevInto.EnableWindow(TRUE);
    m_buttonNextInto.EnableWindow(TRUE);
    m_buttonProcStart.EnableWindow(TRUE);
    m_buttonProcEnd.EnableWindow(TRUE);
    m_buttonPrevSnippet.EnableWindow(TRUE);
    m_buttonNextSnippet.EnableWindow(TRUE);
    m_buttonPrev.EnableWindow(TRUE);
    m_buttonNext.EnableWindow(TRUE);
    m_buttonToEnd.EnableWindow(TRUE);


    CString note;
    int count = RecordEnd - RecordStart;
    note.Format(TEXT("Fetched Branch Records!\r\n Branch count: %d\r\nInstrction count: %d"), count, instrCount);
    MessageBox(note, TEXT("SUCCESS"));



    // 释放内存
    if (RecordStart) {
        VirtualFree((PVOID)RecordStart, 0, MEM_RELEASE);
        RecordStart = NULL;
        RecordEnd = NULL;
    }

}

void ThreadTraceDialog::ResetAll()
{
    // 停止监控
    StopTrace();
    m_bTracingFlag = FALSE;

    // 重置控件
    m_buttonStarttrace.EnableWindow(TRUE);
    m_buttonReset.EnableWindow(FALSE);
    m_buttonPrevInto.EnableWindow(FALSE);
    m_buttonNextInto.EnableWindow(FALSE);
    m_buttonProcStart.EnableWindow(FALSE);
    m_buttonProcEnd.EnableWindow(FALSE);
    m_buttonPrevSnippet.EnableWindow(FALSE);
    m_buttonNextSnippet.EnableWindow(FALSE);
    m_buttonPrev.EnableWindow(FALSE);
    m_buttonNext.EnableWindow(FALSE);
    m_buttonToEnd.EnableWindow(FALSE);

    m_comboThread.EnableWindow(TRUE);
    m_comboThread.SetCurSel(-1);
    m_comboModule.EnableWindow(TRUE);
    m_comboModule.SetCurSel(-1);



}

void ThreadTraceDialog::HighlightTrace()
{
    //PBTS_RECORD64 pRecord = RecordCursor;
    //CString str;
    //str.Format("traceexecute %p", pRecord->to);
    //DbgCmdExec(str.GetBuffer());
    //str.Format("traceexecute %p", pRecord->from);
    //DbgCmdExec(str.GetBuffer());

    return;
}


