#pragma once
#include "dllmain.h"

#include "pluginsdk/_plugins.h"
#include "pluginsdk/bridgemain.h"


#ifndef DLL_EXPORT
#define DLL_EXPORT __declspec(dllexport)
#endif //DLL_EXPORT


#define PLUGIN_NAME "Branch Trace"
#define PLUGIN_VERSION 1

enum AS_MENU
{
    AS_MENU_THREAD_TRACE,
    AS_MENU_TEST
};

#ifdef __cplusplus
extern "C" {
#endif
/* �����ʼ���ɹ���ú����ᱻ���� */
DLL_EXPORT void plugsetup(PLUG_SETUPSTRUCT* setupStruct);

/* �����ж�غ�ú����ᱻ���� */
DLL_EXPORT bool plugstop();

/*�����ʼ��ʱ������*/
DLL_EXPORT bool pluginit(PLUG_INITSTRUCT* setupStruct);

/* �˵��������ú����ᱻ���� */
DLL_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info);

/* �������ж�ʱ��x64dbg����*/
void OnBreakPoint(CBTYPE cbType, void* callbackInfo);
void OnInitDebug(CBTYPE cbType, void* callbackInfo);
void OnStopDebug(CBTYPE cbType, void* callbackInfo);

#ifdef __cplusplus
}
#endif


extern HMODULE g_hModul; 
extern HWND hwndX64Dbg;

