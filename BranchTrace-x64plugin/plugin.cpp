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


///*�����ʼ��ʱ������*/
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

/* �����ʼ���ɹ���ú����ᱻ���� */
DLL_EXPORT void plugsetup(PLUG_SETUPSTRUCT* setupStruct)
{
    g_hwndX64Dbg = setupStruct->hwndDlg;
    hMenu = setupStruct->hMenu;
    hMenuDump = setupStruct->hMenuDump;
    bool result = true;


    // ע��˵���
    _plugin_menuaddentry(hMenu, MENU_THREAD_TRACE, "Thread Trace");
    //_plugin_menuaddentry(hMenu, MENU_POINTER_SCANNER, "Pointer Scanner");

    // ����ͼ��
    ICONDATA icon;
    HRSRC hResPng;
    HGLOBAL hResLoad;

    // ����һ�����˵�ͼ��
    hResPng = FindResource(g_hModule, MAKEINTRESOURCE(IDB_PNG_TRACE), TEXT("PNG"));
    hResLoad = LoadResource(g_hModule, hResPng);
    icon.data = LockResource(hResLoad);
    icon.size = SizeofResource(g_hModule, hResPng);
    _plugin_menuseticon(hMenu, (const ICONDATA*)&icon);

    // ���ö����˵�ͼ��
    hResPng = FindResource(g_hModule, MAKEINTRESOURCE(IDB_PNG_THREAD), TEXT("PNG"));
    hResLoad = LoadResource(g_hModule, hResPng);
    icon.data = LockResource(hResLoad);
    icon.size = SizeofResource(g_hModule, hResPng);
    _plugin_menuentryseticon(pluginHandle, MENU_THREAD_TRACE, &icon);

}

/* �����ж�غ�ú����ᱻ���� */
DLL_EXPORT bool plugstop()
{
    _plugin_menuentryremove(pluginHandle, MENU_THREAD_TRACE);
    return true;
}


/* �˵��������ú����ᱻ���� */
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
