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

            //  调用一次让这个模块增加一次引用计数. 否者卸载dll时会出错, 让其
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
