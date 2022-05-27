// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include "HashTable.h"
#include "Element.h"

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

extern "C"
{ __declspec(dllexport) int Sum (int a, int b) { return (a + b); }
}