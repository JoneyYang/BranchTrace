#include "pch.h"
#include "plugin.h"
#include "Resource.h"
#include "plugin_global.h"

//#include "resource.h"
//#include "plugin_global.h"

int hMenu;
int hMenuDump;
int pluginHandle;
HWND g_hwndX64Dbg;

//HANDLE hBatchCommandThread;
//BatchCommandDlg* dlg;

void MenuOffsetSearch();
void ShowDialogThreadTrace();


///*插件初始化时被调用*/
DLL_EXPORT bool pluginit(PLUG_INITSTRUCT* initStruct)
{
    pluginHandle = initStruct->pluginHandle;
    initStruct->sdkVersion = PLUG_SDKVERSION;
    initStruct->pluginVersion = PLUGIN_VERSION;
    strcpy_s(initStruct->pluginName, PLUGIN_NAME);
    _plugin_registercommand(pluginHandle, PLUGIN_NAME, NULL, false);

    //_plugin_registercallback(pluginHandle, CB_INITDEBUG, OnInitDebug);
    //_plugin_registercallback(pluginHandle, CB_BREAKPOINT, OnBreakPoint);

    return true;
}

/* 插件初始化成功后该函数会被调用 */
DLL_EXPORT void plugsetup(PLUG_SETUPSTRUCT* setupStruct)
{
    g_hwndX64Dbg = setupStruct->hwndDlg;
    hMenu = setupStruct->hMenu;
    hMenuDump = setupStruct->hMenuDump;
    bool result = true;


    // 注册菜单栏
    _plugin_menuaddentry(hMenu, MENU_THREAD_TRACE, "Thread Trace");
    //_plugin_menuaddentry(hMenu, MENU_POINTER_SCANNER, "Pointer Scanner");

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
    _plugin_menuentryseticon(pluginHandle, MENU_THREAD_TRACE, &icon);

}

/* 插件被卸载后该函数会被调用 */
DLL_EXPORT bool plugstop()
{
    _plugin_menuentryremove(pluginHandle, MENU_THREAD_TRACE);
    return true;
}


/* 菜单被点击后该函数会被调用 */
DLL_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
    switch (info->hEntry)
    {
        case MENU_THREAD_TRACE:
        {
            ShowDialogThreadTrace();
            break;
        }
        default:
            break;
    }
}

void ShowDialogThreadTrace()
{
    PluginGlobal::GetInstance()->ShowDlgThreadTrace();
}
