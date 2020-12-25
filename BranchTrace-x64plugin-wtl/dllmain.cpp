#include "dllmain.h"
#include "plugin_global.h"

HMODULE g_hModule;
HANDLE g_hThread;

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            g_hModule = hModule;

            //  ����һ�������ģ������һ�����ü���. ����ж��dllʱ�����, ����
            HMODULE moudle;
            GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPSTR)DllMain, &moudle);

            break;
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}
