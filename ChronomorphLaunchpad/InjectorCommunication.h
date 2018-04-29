#pragma once

#include <Windows.h>
#include "stdafx.h"

#include <iostream>

// Creates server side pipe.
// This pipe is meant to be passed to the child process
// and used for passing the input arguments.
//
bool CreateLaunchpadPipe(HANDLE& hReadRead, HANDLE& hWriteHandle)
{
	HANDLE hReadPipe;
	HANDLE hWritePipe;
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = true;
	bool bOk;

	bOk = CreatePipe(
		&hReadPipe,
		&hWritePipe,
		&sa,
		0 /* Default buffer size */);

	if (!bOk)
	{
		if (DebugMode)
		{
			std::wcout << "Failure to open server side pipe." << std::endl;
		}
	}

	return bOk;
}
