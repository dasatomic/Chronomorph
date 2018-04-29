#include "stdafx.h"
#include <Windows.h>
#include "APIHook.h"
#include "ChronoModeEnum.h"
#include "TimeOperations.h"

#include <vector>

struct InputArgs
{
	ChronoModeEnum chronoMode;
	int offset = 0;
};

std::vector<CAPIHook*> g_timeHooks;
InputArgs g_InputArgs;

void WINAPI GetLocalTime_Hook(_Out_ LPSYSTEMTIME lpSystemTime)
{
	::GetLocalTime(lpSystemTime);

	*lpSystemTime = AddSecondsOnSystemTime(*lpSystemTime, static_cast<double>(g_InputArgs.offset));
}

void WINAPI GetSystemTime_Hook(_Out_ LPSYSTEMTIME lpSystemTime)
{
	::GetSystemTime(lpSystemTime);
	*lpSystemTime = AddSecondsOnSystemTime(*lpSystemTime, static_cast<double>(g_InputArgs.offset));
}

void InitializeTimeHooks()
{
	wchar_t envMode[MAX_PATH];
	GetEnvironmentVariable(CHRONO_MODE_ARG, envMode, MAX_PATH);
	g_InputArgs.chronoMode = static_cast<ChronoModeEnum>(_wtoi(envMode));

	wchar_t envArg[MAX_PATH];
	GetEnvironmentVariable(CHRONO_MODE_ARG_1, envArg, MAX_PATH);

	if (g_InputArgs.chronoMode == ChronoModeEnum::TimeOffset)
	{
		int offset = _wtoi(envArg);
		g_InputArgs.offset = offset;

		CAPIHook* getLocalTimeHook = new CAPIHook("Kernel32.dll", "GetLocalTime", (PROC)GetLocalTime_Hook);
		CAPIHook* getSystemTimeHook = new CAPIHook("Kernel32.dll", "GetSystemTime", (PROC)GetSystemTime_Hook);

		g_timeHooks.push_back(getLocalTimeHook);
		g_timeHooks.push_back(getSystemTimeHook);
	}
}