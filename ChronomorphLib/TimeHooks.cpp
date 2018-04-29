#include "stdafx.h"
#include <Windows.h>
#include "APIHook.h"
#include "ChronoModeEnum.h"

CAPIHook CAPIHook::sm_GetLocalTime("Kernel32.dll", "GetLocalTime", (PROC)CAPIHook::GetLocalTime);

void WINAPI CAPIHook::GetLocalTime(_Out_ LPSYSTEMTIME lpSystemTime)
{
	::GetLocalTime(lpSystemTime);

	wchar_t envMode[MAX_PATH];
	GetEnvironmentVariable(CHRONO_MODE_ARG, envMode, MAX_PATH);

	wchar_t envArg[MAX_PATH];
	GetEnvironmentVariable(CHRONO_MODE_ARG_1, envArg, MAX_PATH);

	int offset = _wtoi(envArg);

	// TODO: Hard coding for initial testing.
	// This should be passed as an argument from time morpher.
	//
	lpSystemTime->wHour = lpSystemTime->wHour + offset;
}

CAPIHook CAPIHook::sm_GetSystemTime("Kernel32.dll", "GetSytemTime", (PROC)CAPIHook::GetSystemTime);

void WINAPI CAPIHook::GetSystemTime(_Out_ LPSYSTEMTIME lpSystemTime)
{
	::GetSystemTime(lpSystemTime);

	// TODO: Hard coding for initial testing.
	// This should be passed as an argument from time morpher.
	//
	// lpSystemTime->wHour = lpSystemTime->wHour - 5;
}
