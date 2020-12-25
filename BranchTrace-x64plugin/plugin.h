#pragma once

#include <windows.h>
#include "pluginsdk/_plugins.h"
#include "pluginsdk/bridgemain.h"


#define PLUGIN_NAME "Branch Trace"
#define PLUGIN_VERSION 1

#ifndef DLL_EXPORT
#define DLL_EXPORT __declspec(dllexport)
#endif //DLL_EXPORT


enum PLUGIN_MENUS
{
    MENU_THREAD_TRACE
};

EXTERN_C_START

/* 插件初始化成功后该函数会被调用 */
DLL_EXPORT void plugsetup(PLUG_SETUPSTRUCT* setupStruct);
/* 插件被卸载后该函数会被调用 */
DLL_EXPORT bool plugstop();
/* 菜单被点击后该函数会被调用 */
DLL_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info);
/*插件初始化时被调用*/
DLL_EXPORT bool pluginit(PLUG_INITSTRUCT* setupStruct);

EXTERN_C_END

extern HMODULE g_hModule;
extern HWND g_hwndX64Dbg;



