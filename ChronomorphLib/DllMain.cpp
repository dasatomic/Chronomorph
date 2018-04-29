#include "stdafx.h"
#include <Windows.h>

void InitializeTimeHooks();

bool WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, PVOID fImpLoad) 
{
	OutputDebugString(L"Entering Dll main.");

	switch (fdwReason) 
	{

	case DLL_THREAD_ATTACH:
		break;

	case DLL_PROCESS_ATTACH:
		InitializeTimeHooks();
		break;

	case DLL_THREAD_DETACH:

	case DLL_PROCESS_DETACH:
		break;
	}
	return(true);
}