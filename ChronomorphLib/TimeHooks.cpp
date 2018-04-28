#include "stdafx.h"
#include <Windows.h>
#include "APIHook.h"

CAPIHook CAPIHook::sm_GetLocalTime("Kernel32.dll", "GetLocalTime", (PROC)CAPIHook::GetLocalTime);

void WINAPI CAPIHook::GetLocalTime(_Out_ LPSYSTEMTIME lpSystemTime)
{
	::GetLocalTime(lpSystemTime);

	// TODO: Hard coding for initial testing.
	// This should be passed as an argument from time morpher.
	//
	lpSystemTime->wHour = lpSystemTime->wHour - 5;
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
