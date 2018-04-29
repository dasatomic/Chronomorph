#include "stdafx.h"
#include <Windows.h>
#include "APIHook.h"
#include "ChronoModeEnum.h"

#include <vector>

struct InputArgs
{
	int offset = 0;
};

std::vector<CAPIHook*> g_timeHooks;
InputArgs g_InputArgs;

void WINAPI GetLocalTime_Hook(_Out_ LPSYSTEMTIME lpSystemTime)
{
	::GetLocalTime(lpSystemTime);

	int offset = g_InputArgs.offset;
	lpSystemTime->wHour = lpSystemTime->wHour + offset;
}

void WINAPI GetSystemTime_Hook(_Out_ LPSYSTEMTIME lpSystemTime)
{
	::GetSystemTime(lpSystemTime);
}

void InitializeTimeHooks()
{
	wchar_t envMode[MAX_PATH];
	GetEnvironmentVariable(CHRONO_MODE_ARG, envMode, MAX_PATH);
	ChronoModeEnum chronoMode = static_cast<ChronoModeEnum>(_wtoi(envMode));

	wchar_t envArg[MAX_PATH];
	GetEnvironmentVariable(CHRONO_MODE_ARG_1, envArg, MAX_PATH);

	if (chronoMode == ChronoModeEnum::TimeOffset)
	{
		int offset = _wtoi(envArg);
		g_InputArgs.offset = offset;

		CAPIHook* getLocalTimeHook = new CAPIHook("Kernel32.dll", "GetLocalTime", (PROC)GetLocalTime_Hook);
		CAPIHook* getSystemTimeHook = new CAPIHook("Kernel32.dll", "GetSystemTime", (PROC)GetSystemTime_Hook);

		g_timeHooks.push_back(getLocalTimeHook);
		g_timeHooks.push_back(getSystemTimeHook);
	}
}