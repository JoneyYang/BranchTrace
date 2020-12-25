#pragma once
#include "stdafx.h"
#include "ThreadTraceDialog.h"
#include "plugin.h"
#include "disassembler.h"

class PluginGlobal {

public:
    ThreadTraceDialog* threadTraceDialog;
    Disassembler* disassembler;

private:
    PluginGlobal() {
        disassembler = new Disassembler();
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
        if (threadTraceDialog == NULL) {
            threadTraceDialog = new ThreadTraceDialog;
            threadTraceDialog->Create(hwndX64Dbg);
        }

        threadTraceDialog->ShowWindow(SW_SHOW);
        threadTraceDialog->SetActiveWindow();
    };

    void CloseAll() {
        if (threadTraceDialog) {
            CloseWindow(threadTraceDialog->m_hWnd);
            DestroyWindow(threadTraceDialog->m_hWnd);
        }
    }
};
