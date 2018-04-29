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
	DWORD RelativeTimeGetTimeCapture;
	DWORD RelativeTimeGetTimeSystemCapture;
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
	assert(g_InputArgs.chronoMode == ChronoModeEnum::TimeFlowRatio || g_InputArgs.chronoMode == ChronoModeEnum::TimeOffset);

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

UINT_PTR WINAPI SetTimer_Hook(
	_In_opt_ HWND      hWnd,
	_In_     UINT_PTR  nIDEvent,
	_In_     UINT      uElapse,
	_In_opt_ TIMERPROC lpTimerFunc
)
{
	// Valid only in timeflow ratio mode.
	//
	assert(g_InputArgs.chronoMode == ChronoModeEnum::TimeFlowRatio);

	uElapse = uElapse / g_InputArgs.ratio;
	return ::SetTimer(hWnd, nIDEvent, uElapse, lpTimerFunc);
}

VOID WINAPI SetThreadpoolTimer_Hook(
	_Inout_  PTP_TIMER pti,
	_In_opt_ PFILETIME pftDueTime,
	_In_     DWORD     msPeriod,
	_In_opt_ DWORD     msWindowLength
)
{
	// Valid only in timeflow ratio mode.
	//
	assert(g_InputArgs.chronoMode == ChronoModeEnum::TimeFlowRatio);

	msPeriod = static_cast<double>(msPeriod) / g_InputArgs.ratio;

	if (pftDueTime != nullptr)
	{
		ULARGE_INTEGER uDueTime;
		memcpy(&uDueTime, pftDueTime, sizeof(uDueTime));
		uDueTime.QuadPart = uDueTime.QuadPart / g_InputArgs.ratio;

		FILETIME modifiedDueTime;
		memcpy(&modifiedDueTime, &uDueTime, sizeof(modifiedDueTime));
		return ::SetThreadpoolTimer(pti, &modifiedDueTime, msPeriod, msWindowLength);
	}
	else
	{
		return ::SetThreadpoolTimer(pti, pftDueTime, msPeriod, msWindowLength);
	}

}

BOOL WINAPI CreateTimerQueueTimer_Hook(
	_Out_    PHANDLE             phNewTimer,
	_In_opt_ HANDLE              TimerQueue,
	_In_     WAITORTIMERCALLBACK Callback,
	_In_opt_ PVOID               Parameter,
	_In_     DWORD               DueTime,
	_In_     DWORD               Period,
	_In_     ULONG               Flags
)
{
	assert(g_InputArgs.chronoMode == ChronoModeEnum::TimeFlowRatio);
	Period /= g_InputArgs.ratio;
	DueTime /= g_InputArgs.ratio;

	return ::CreateTimerQueueTimer_Hook(phNewTimer, TimerQueue, Callback, Parameter, DueTime, Period, Flags);
}

VOID WINAPI Sleep_Hook(
	_In_ DWORD dwMilliseconds
)
{
	// Valid only in timeflow ratio mode.
	//
	assert(g_InputArgs.chronoMode == ChronoModeEnum::TimeFlowRatio);

	dwMilliseconds = dwMilliseconds / g_InputArgs.ratio;
	return ::Sleep(dwMilliseconds);
}

void WINAPI GetSystemTimeAsFileTime_Hook(
	_Out_ LPFILETIME lpSystemTimeAsFileTime
)
{
	assert(g_InputArgs.chronoMode == ChronoModeEnum::TimeFlowRatio);

	SYSTEMTIME st;
	GetSystemTime_Hook(&st);
	FileTimeToSystemTime(lpSystemTimeAsFileTime, &st);
}

DWORD timeGetTime_Hook(void)
{
	assert(g_InputArgs.chronoMode == ChronoModeEnum::TimeFlowRatio || g_InputArgs.chronoMode == ChronoModeEnum::TimeOffset);

	DWORD time = ::timeGetTime();
	return g_Context.RelativeTimeGetTimeCapture + ((time - g_Context.RelativeTimeGetTimeCapture) * g_InputArgs.ratio);
}

// TODO: This seems to be the right thing to override.
// https://msdn.microsoft.com/en-us/library/windows/desktop/dd757629(v=vs.85).aspx

// MMRESULT timeGetSystemTime_Hook(LPMMTIME pmmt, UINT cbmmt)
// {
// 	assert(g_InputArgs.chronoMode == ChronoModeEnum::TimeFlowRatio || g_InputArgs.chronoMode == ChronoModeEnum::TimeOffset);

// 	MMRESULT res = ::timeGetSystemTime(pmmt, cbmmt);

// 	if (res == TIMERR_NOERROR)
// 	{
// 		// TODO: Mock should happen here, not sure what to do.
// 		//
// 	}

// 	return res;
// }

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

		CAPIHook* sleepHook = new CAPIHook("Kernel32.dll", "Sleep", (PROC)Sleep_Hook);
		CAPIHook* setTimerHook = new CAPIHook("User32.dll", "SetTimer", (PROC)SetTimer_Hook);
		CAPIHook* systemTimeAsFileTimeHook = new CAPIHook("Kernel32.dll", "GetSystemTimeAsFileTime", (PROC)GetSystemTimeAsFileTime_Hook);
		CAPIHook* timeGetTimeHook = new CAPIHook("Kernel32.dll", "timeGetTime", (PROC)timeGetTime_Hook);
		CAPIHook* timeGetTimeHook_Winmm = new CAPIHook("Winmm.dll", "timeGetTime", (PROC)timeGetTime_Hook);
		CAPIHook* createTimerQueueTimerHook = new CAPIHook("Kernel32.dll", "CreateTimerQueueTimer", (PROC)CreateTimerQueueTimer_Hook);
		CAPIHook* setThreadpoolTimerHook = new CAPIHook("Kernel32.dll", "SetThreadpoolTimer", (PROC)SetThreadpoolTimer_Hook);

		g_timeHooks.push_back(getLocalTimeHook);
		g_timeHooks.push_back(getSystemTimeHook);
		g_timeHooks.push_back(sleepHook);
		g_timeHooks.push_back(setTimerHook);
		g_timeHooks.push_back(systemTimeAsFileTimeHook);
		g_timeHooks.push_back(timeGetTimeHook);
		g_timeHooks.push_back(timeGetTimeHook_Winmm);
		g_timeHooks.push_back(createTimerQueueTimerHook);
		g_timeHooks.push_back(setThreadpoolTimerHook);
	}
}