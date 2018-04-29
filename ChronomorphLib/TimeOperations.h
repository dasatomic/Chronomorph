#pragma once

#include "stdafx.h"
#include <Windows.h>

SYSTEMTIME AddSecondsOnSystemTime(SYSTEMTIME systemTime, double seconds) {
	FILETIME fileTime;
	SystemTimeToFileTime(&systemTime, &fileTime);

	ULARGE_INTEGER u;
	memcpy(&u, &fileTime, sizeof(u));

	const double c_dSecondsPer100nsInterval = 100.*1.e-9;
	const double c_dNumberOf100nsIntervals =
		seconds / c_dSecondsPer100nsInterval;

	// note: you may want to round the number of intervals.
	u.QuadPart += c_dNumberOf100nsIntervals;

	memcpy(&fileTime, &u, sizeof(fileTime));

	FileTimeToSystemTime(&fileTime, &systemTime);
	return systemTime;
}
