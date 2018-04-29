#pragma once

#include "stdafx.h"
#include <Windows.h>

SYSTEMTIME AlterSystemTimeFlow(
	SYSTEMTIME currentRealTime,
	SYSTEMTIME relativeTimePoint,
	double ratio)
{
	// TODO: This is going to be too slow, you can do better.
	//
	FILETIME ftCurrentRealTime;
	FILETIME ftRelativeTimePoint;
	SystemTimeToFileTime(&currentRealTime, &ftCurrentRealTime);
	SystemTimeToFileTime(&relativeTimePoint, &ftRelativeTimePoint);

	ULARGE_INTEGER uCurrentRealTime;
	memcpy(&uCurrentRealTime, &ftCurrentRealTime, sizeof(uCurrentRealTime));

	// TODO: Cache this.
	//
	ULARGE_INTEGER uRelativeTimePoint;
	memcpy(&uRelativeTimePoint, &ftRelativeTimePoint, sizeof(uRelativeTimePoint));

	assert(uCurrentRealTime.QuadPart >= uRelativeTimePoint.QuadPart);
	LONGLONG llTimePassed = uCurrentRealTime.QuadPart - uRelativeTimePoint.QuadPart;

	// TODO: Is this safe?
	//
	llTimePassed = static_cast<long long>(llTimePassed * ratio);
	uCurrentRealTime.QuadPart = uRelativeTimePoint.QuadPart + llTimePassed;
	memcpy(&ftCurrentRealTime, &uCurrentRealTime, sizeof(ftCurrentRealTime));
	FileTimeToSystemTime(&ftCurrentRealTime, &currentRealTime);

	return currentRealTime;
}

SYSTEMTIME AddSecondsOnSystemTime(SYSTEMTIME systemTime, double seconds)
{
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
