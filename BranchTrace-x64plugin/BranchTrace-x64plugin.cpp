// BranchTrace-x64plugin.cpp : Defines the initialization routines for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "BranchTrace-x64plugin.h"
#include "plugin.h"
#include "ThreadTraceDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CBranchTracex64pluginApp

BEGIN_MESSAGE_MAP(CBranchTracex64pluginApp, CWinApp)
END_MESSAGE_MAP()


// CBranchTracex64pluginApp construction

CBranchTracex64pluginApp::CBranchTracex64pluginApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CBranchTracex64pluginApp object

CBranchTracex64pluginApp theApp;


// CBranchTracex64pluginApp initialization

HMODULE g_hModule;
BOOL CBranchTracex64pluginApp::InitInstance()
{
	CWinApp::InitInstance();
	g_hModule =this->m_hInstance;

	return TRUE;
}
