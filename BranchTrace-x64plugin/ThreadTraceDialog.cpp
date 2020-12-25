// ThreadTraceDialog.cpp : implementation file
//

#include "pch.h"
#include "BranchTrace-x64plugin.h"
#include "ThreadTraceDialog.h"
#include "afxdialogex.h"
#include "pluginsdk/bridgemain.h"
#include "pluginsdk/_plugins.h"


// ThreadTraceDialog dialog

IMPLEMENT_DYNAMIC(CThreadTraceDialog, CDialog)

CThreadTraceDialog::CThreadTraceDialog(CWnd* pParent /*=nullptr*/)
    : CDialog(IDD_DIALOG_THREAD_TRACE, pParent)
{

}

CThreadTraceDialog::~CThreadTraceDialog()
{
}

void CThreadTraceDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_THREAD, threadCombo);
}


BEGIN_MESSAGE_MAP(CThreadTraceDialog, CDialog)
    ON_CBN_DROPDOWN(IDC_COMBO_THREAD, &CThreadTraceDialog::OnCbnDropdownComboThread)
END_MESSAGE_MAP()


// ThreadTraceDialog message handlers


void CThreadTraceDialog::OnCbnDropdownComboThread()
{
    // TODO: Add your control notification handler code here
    THREADLIST threadList;
    DbgGetThreadList(&threadList);

    threadCombo.ResetContent();

    for (size_t i = 0; i < threadList.count; i++)
    {
        THREADALLINFO* thread = &threadList.list[i];

        bool isMainThread = thread->BasicInfo.ThreadNumber == 0;
        bool isCurrentThead = i == threadList.CurrentThread;

        CString str;
        str.Format("[%d] thread-%x %s %s", i, thread->BasicInfo.ThreadId, isMainThread ? "[main]" : "", isCurrentThead ? "[current]" : "");
        threadCombo.InsertString(i, str);
        threadCombo.SetItemData(i, thread->BasicInfo.ThreadId);

        if (isMainThread) {
            threadCombo.SetCurSel(i);
        }

    }
}
