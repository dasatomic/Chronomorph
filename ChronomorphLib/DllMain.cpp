#include <Windows.h>

bool WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, PVOID fImpLoad) 
{
	OutputDebugString(L"Entering Dll main.");

	switch (fdwReason) {

	case DLL_THREAD_ATTACH:
		break;

	case DLL_PROCESS_ATTACH:
		break;

	case DLL_THREAD_DETACH:

	case DLL_PROCESS_DETACH:
		// TODO: Do all the cleanup here.
		//
		break;
	}
	return(TRUE);
}