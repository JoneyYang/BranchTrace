#include "plugin.h"
#include "dllmain.h"
#include "stdafx.h"
#include "plugin_global.h"
HWND hwndX64Dbg;
int hMenu;
int hMenuDump;
int pluginHandle;

/*插件初始化时被调用*/
DLL_EXPORT bool pluginit(PLUG_INITSTRUCT* initStruct)
{
    pluginHandle = initStruct->pluginHandle;
    initStruct->sdkVersion = PLUG_SDKVERSION;
    initStruct->pluginVersion = PLUGIN_VERSION;
    strcpy_s(initStruct->pluginName, PLUGIN_NAME);
    _plugin_registercommand(pluginHandle, PLUGIN_NAME, NULL, false);
    return true;
}


/* 插件初始化成功后该函数会被调用 */
DLL_EXPORT void plugsetup(PLUG_SETUPSTRUCT* setupStruct)
{
    hwndX64Dbg = setupStruct->hwndDlg;
    hMenu = setupStruct->hMenu;
    hMenuDump = setupStruct->hMenuDump;
    bool result = true;

    // 注册事件回调
    _plugin_registercallback(pluginHandle, CB_BREAKPOINT, OnBreakPoint);
    _plugin_registercallback(pluginHandle, CB_INITDEBUG, OnInitDebug);
    _plugin_registercallback(pluginHandle, CB_STOPDEBUG, OnStopDebug);


    // 注册菜单栏
    _plugin_menuaddentry(hMenu, AS_MENU_THREAD_TRACE, "Branch Trace");
    _plugin_menuaddentry(hMenu, AS_MENU_TEST, "TEST");

    // 设置图标
    ICONDATA icon;
    HRSRC hResPng;
    HGLOBAL hResLoad;


    // 设置一级主菜单图标
    hResPng = FindResource(g_hModule, MAKEINTRESOURCE(IDB_PNG_TRACE), TEXT("PNG"));
    hResLoad = LoadResource(g_hModule, hResPng);
    icon.data = LockResource(hResLoad);
    icon.size = SizeofResource(g_hModule, hResPng);
    _plugin_menuseticon(hMenu, (const ICONDATA*)&icon);

    // 设置二级菜单图标
    hResPng = FindResource(g_hModule, MAKEINTRESOURCE(IDB_PNG_THREAD), TEXT("PNG"));
    hResLoad = LoadResource(g_hModule, hResPng);
    icon.data = LockResource(hResLoad);
    icon.size = SizeofResource(g_hModule, hResPng);
    _plugin_menuentryseticon(pluginHandle, AS_MENU_THREAD_TRACE, &icon);
}

/* 插件被卸载后该函数会被调用 */
DLL_EXPORT bool plugstop()
{
    _plugin_menuentryremove(pluginHandle, AS_MENU_THREAD_TRACE);

    PluginGlobal* instance = PluginGlobal::GetInstance();
    instance->CloseAll();

    return true;
}

/* 菜单被点击后该函数会被调用 */
DLL_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
    unsigned char buf[0x10] = { 0 };
    switch (info->hEntry)
    {
        case AS_MENU_THREAD_TRACE:
        {
            if (DbgIsDebugging()) {
                PluginGlobal* instance = PluginGlobal::GetInstance();
                instance->ShowDlgThreadTrace();
            }
            else
            {
                MessageBox(hwndX64Dbg, TEXT("Not Debugging..."), TEXT("Error"), MB_OK);

            }
            break;
        }

        case AS_MENU_TEST:
        {
            PluginGlobal* instance = PluginGlobal::GetInstance();
            if (instance->threadTraceDialog) {
                instance->threadTraceDialog->OnBreakDebugPoint();
            }
            break;
        }
        default:
            break;
    }
}


void OnBreakPoint(CBTYPE cbType, void* callbackInfo) {
    //BRIDGEBP* breakpointInfo = ((PLUG_CB_BREAKPOINT*)callbackInfo)->breakpoint;
    PluginGlobal* instance = PluginGlobal::GetInstance();
    if (instance->threadTraceDialog) {
        instance->threadTraceDialog->OnBreakDebugPoint();
    }

}

void OnInitDebug(CBTYPE cbType, void* callbackInfo) {


}

void OnStopDebug(CBTYPE cbType, void* callbackInfo) {

    PluginGlobal::GetInstance()->disassembler->Reset();
}
