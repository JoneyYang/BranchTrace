// BranchTrace-x64plugin.h : main header file for the BranchTrace-x64plugin DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CBranchTracex64pluginApp
// See BranchTrace-x64plugin.cpp for the implementation of this class
//

class CBranchTracex64pluginApp : public CWinApp
{
public:
	CBranchTracex64pluginApp();

// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
