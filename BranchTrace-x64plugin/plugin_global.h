#pragma once
#include "pch.h"
#include "ThreadTraceDialog.h"

class PluginGlobal {

public:
    CThreadTraceDialog* threadTraceDialog;

private:
    PluginGlobal() :threadTraceDialog(NULL) {
        AFX_MANAGE_STATE(AfxGetStaticModuleState());
        threadTraceDialog = new CThreadTraceDialog();
    };

public:
    ~PluginGlobal() {
        CloseAll();
    }

public:
    static PluginGlobal* GetInstance() {
        static PluginGlobal pluginGlobal;
        return &pluginGlobal;
    }

    void ShowDlgThreadTrace() {
        auto showdlg = [](LPVOID data) -> DWORD {
            AFX_MANAGE_STATE(AfxGetStaticModuleState());
            PluginGlobal* pluginglobal = (PluginGlobal*)data;
            pluginglobal->threadTraceDialog->DoModal();
            return 0;
        };

        AFX_MANAGE_STATE(AfxGetStaticModuleState());
        HANDLE thread = CreateThread(NULL, 0, showdlg, this, 0, NULL);
        if (thread != NULL) {
            CloseHandle(thread);
        }
    };

    void CloseAll() {
        SendMessage(threadTraceDialog->m_hWnd, WM_CLOSE, 0, 0);
    }
};
