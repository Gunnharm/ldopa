// dllmain.cpp : Defines the entry point for the DLL application.



#include "stdafx.h"
#include <windows.h>                    // добавил 28.10.2017, т.к. иначе на находит APIENTRY
                                        // вообще, т.к. быть не должно: см. LDOPA, но где-то 
                                        // не хватает... надобно разобраться

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}



