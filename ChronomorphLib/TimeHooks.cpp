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
	double ratio = 1.0;
};

std::vector<CAPIHook*> g_timeHooks;
InputArgs g_InputArgs;

struct Context
{
	SYSTEMTIME RelativeSystemTimeCapture;
	SYSTEMTIME RelativeLocalTimeCapture;
};

Context g_Context;

void WINAPI GetLocalTime_Hook(_Out_ LPSYSTEMTIME lpSystemTime)
{
	::GetLocalTime(lpSystemTime);

	if (g_InputArgs.chronoMode == ChronoModeEnum::TimeOffset)
	{
		*lpSystemTime = AddSecondsOnSystemTime(*lpSystemTime, static_cast<double>(g_InputArgs.offset));
	}
	else if (g_InputArgs.chronoMode == ChronoModeEnum::TimeFlowRatio)
	{
		*lpSystemTime = AlterSystemTimeFlow(*lpSystemTime, g_Context.RelativeLocalTimeCapture, g_InputArgs.ratio);
	}
}

void WINAPI GetSystemTime_Hook(_Out_ LPSYSTEMTIME lpSystemTime)
{
	::GetSystemTime(lpSystemTime);

	if (g_InputArgs.chronoMode == ChronoModeEnum::TimeOffset)
	{
		*lpSystemTime = AddSecondsOnSystemTime(*lpSystemTime, static_cast<double>(g_InputArgs.offset));
	}
	else if (g_InputArgs.chronoMode == ChronoModeEnum::TimeFlowRatio)
	{
		*lpSystemTime = AlterSystemTimeFlow(*lpSystemTime, g_Context.RelativeSystemTimeCapture, g_InputArgs.ratio);
	}
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
		g_InputArgs.offset = _wtoi(envArg);

		CAPIHook* getLocalTimeHook = new CAPIHook("Kernel32.dll", "GetLocalTime", (PROC)GetLocalTime_Hook);
		CAPIHook* getSystemTimeHook = new CAPIHook("Kernel32.dll", "GetSystemTime", (PROC)GetSystemTime_Hook);

		g_timeHooks.push_back(getLocalTimeHook);
		g_timeHooks.push_back(getSystemTimeHook);
	}
	else if (g_InputArgs.chronoMode == ChronoModeEnum::TimeFlowRatio)
	{
		g_InputArgs.ratio = _wtof(envArg);

		::GetSystemTime(&g_Context.RelativeSystemTimeCapture);
		::GetLocalTime(&g_Context.RelativeLocalTimeCapture);

		CAPIHook* getLocalTimeHook = new CAPIHook("Kernel32.dll", "GetLocalTime", (PROC)GetLocalTime_Hook);
		CAPIHook* getSystemTimeHook = new CAPIHook("Kernel32.dll", "GetSystemTime", (PROC)GetSystemTime_Hook);

		g_timeHooks.push_back(getLocalTimeHook);
		g_timeHooks.push_back(getSystemTimeHook);
	}
}