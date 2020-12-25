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

/* �����ʼ���ɹ���ú����ᱻ���� */
DLL_EXPORT void plugsetup(PLUG_SETUPSTRUCT* setupStruct);
/* �����ж�غ�ú����ᱻ���� */
DLL_EXPORT bool plugstop();
/* �˵��������ú����ᱻ���� */
DLL_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info);
/*�����ʼ��ʱ������*/
DLL_EXPORT bool pluginit(PLUG_INITSTRUCT* setupStruct);

EXTERN_C_END

extern HMODULE g_hModule;
extern HWND g_hwndX64Dbg;



