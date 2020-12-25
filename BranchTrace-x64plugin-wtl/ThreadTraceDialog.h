#pragma once
#include "stdafx.h"
#include "ThreadTraceCore.h"


class ThreadTraceDialog : public CDialogImpl<ThreadTraceDialog>, public CWinDataExchange<ThreadTraceDialog>
{

protected:

    // ¿Ø¼þ
    CComboBoxImpl m_comboThread;
    CComboBoxImpl m_comboModule;
    CButtonImpl m_buttonStarttrace;
    CButtonImpl m_buttonReset;
    CButtonImpl m_buttonPrevInto;
    CButtonImpl m_buttonNextInto;
    CButtonImpl m_buttonProcStart;
    CButtonImpl m_buttonProcEnd;
    CButtonImpl m_buttonPrevSnippet;
    CButtonImpl m_buttonNextSnippet;
    CButtonImpl m_buttonPrev;
    CButtonImpl m_buttonNext;
    CButtonImpl m_buttonToEnd;


    PBTS_RECORD64 RecordStart = NULL;
    PBTS_RECORD64 RecordEnd = NULL;
    PBTS_RECORD64 RecordCursor = NULL;
    BOOL ToFromFlag = TRUE;
    TraceCursor cursor;


#define AT_TO TRUE
#define AT_FROM FALSE
    BOOL m_bTracingFlag;    // AT_TO | AT_FROM


public:
    ThreadTraceDialog() : RecordStart(NULL), RecordEnd(NULL), m_bTracingFlag(FALSE) {};
    ~ThreadTraceDialog();


    // msg handler
    BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
    void OnCloseDialog();

    // command hander
    LRESULT OnDropdownComboboxThread(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnDropdownComboboxModule(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedButtonStarttrace(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedButtonReset(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedButtonPrevInto(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedButtonNextInto(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedButtonProcStart(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedButtonProcEnd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedButtonPrevSnippet(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedButtonNextSnippet(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedButtonPrev(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedButtonNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
    LRESULT OnBnClickedButtonToEnd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

    void OnBreakDebugPoint();
    void ResetAll();
    void HighlightTrace();


public:
    enum { IDD = IDD_DIALOG_THREAD_TRACE };

    BEGIN_MSG_MAP(ThreadTraceDialog)
        // commands
        MSG_WM_INITDIALOG(OnInitDialog)
        MSG_WM_CLOSE(OnCloseDialog)

        // combo-box
        COMMAND_HANDLER(IDC_COMBO_THREAD, CBN_DROPDOWN, OnDropdownComboboxThread)
        COMMAND_HANDLER(IDC_COMBO_MODULE, CBN_DROPDOWN, OnDropdownComboboxModule)

        // buttons
        COMMAND_ID_HANDLER(IDC_BUTTON_STARTTRACE, OnBnClickedButtonStarttrace)
        COMMAND_ID_HANDLER(IDC_BUTTON_RESET, OnBnClickedButtonReset)
        COMMAND_ID_HANDLER(IDC_BUTTON_PREVINTO, OnBnClickedButtonPrevInto)
        COMMAND_ID_HANDLER(IDC_BUTTON_NEXTINTO, OnBnClickedButtonNextInto)
        COMMAND_ID_HANDLER(IDC_BUTTON_PROCSTART, OnBnClickedButtonProcStart)
        COMMAND_ID_HANDLER(IDC_BUTTON_PROCEND, OnBnClickedButtonProcEnd)
        COMMAND_ID_HANDLER(IDC_BUTTON_PREVSNIPPET, OnBnClickedButtonPrevSnippet)
        COMMAND_ID_HANDLER(IDC_BUTTON_NEXTSNIPPET, OnBnClickedButtonNextSnippet)
        COMMAND_ID_HANDLER(IDC_BUTTON_PREV, OnBnClickedButtonPrev)
        COMMAND_ID_HANDLER(IDC_BUTTON_NEXT, OnBnClickedButtonNext)
        COMMAND_ID_HANDLER(IDC_BUTTON_TOEND, OnBnClickedButtonToEnd)

    END_MSG_MAP()


    BEGIN_DDX_MAP(ThreadTraceDialog)
        // controls
        DDX_CONTROL(IDC_COMBO_THREAD, m_comboThread)
        DDX_CONTROL(IDC_COMBO_MODULE, m_comboModule)
        DDX_CONTROL(IDC_BUTTON_STARTTRACE, m_buttonStarttrace)
        DDX_CONTROL(IDC_BUTTON_RESET, m_buttonReset)
        DDX_CONTROL(IDC_BUTTON_PREVINTO, m_buttonPrevInto)
        DDX_CONTROL(IDC_BUTTON_NEXTINTO, m_buttonNextInto)
        DDX_CONTROL(IDC_BUTTON_PROCSTART, m_buttonProcStart)
        DDX_CONTROL(IDC_BUTTON_PROCEND, m_buttonProcEnd)
        DDX_CONTROL(IDC_BUTTON_PREVSNIPPET, m_buttonPrevSnippet)
        DDX_CONTROL(IDC_BUTTON_NEXTSNIPPET, m_buttonNextSnippet)

        DDX_CONTROL(IDC_BUTTON_PREV, m_buttonPrev)
        DDX_CONTROL(IDC_BUTTON_NEXT, m_buttonNext)
        DDX_CONTROL(IDC_BUTTON_TOEND, m_buttonToEnd)
    END_DDX_MAP()

};