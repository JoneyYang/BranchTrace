#pragma once


// ThreadTraceDialog dialog

class CThreadTraceDialog : public CDialog
{
	DECLARE_DYNAMIC(CThreadTraceDialog)

public:
	CThreadTraceDialog(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CThreadTraceDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_THREAD_TRACE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CComboBox threadCombo;
	afx_msg void OnCbnDropdownComboThread();
};
