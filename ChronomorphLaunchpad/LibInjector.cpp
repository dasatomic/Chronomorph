#include "stdafx.h"
#include <Windows.h>

bool WINAPI InjectLibAtStartup(PWSTR pszExeToStartPath, PCWSTR pszLibToInjectFile)
{
	BOOL bOk = FALSE; // Assume that the function fails
	HANDLE hProcess = nullptr, hThread = nullptr;
	PWSTR pszLibFileRemote = nullptr;

	__try {
		// Start process suspended.
		//
		STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi;

		bOk = CreateProcess(nullptr, pszExeToStartPath, nullptr, nullptr,
			false, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi);

		if (!bOk)
		{
			__leave;
		}

		hProcess = pi.hProcess;

		// Calculate the number of bytes needed for the DLL's pathname
		//
		int cch = 1 + lstrlenW(pszLibToInjectFile);
		int cb = cch * sizeof(wchar_t);

		// Allocate space in the remote process for the pathname
		pszLibFileRemote = (PWSTR)
			VirtualAllocEx(hProcess, nullptr, cb, MEM_COMMIT, PAGE_READWRITE);
		if (pszLibFileRemote == nullptr) __leave;

		// Copy the DLL's pathname to the remote process' address space
		if (!WriteProcessMemory(hProcess, pszLibFileRemote,
			(PVOID)pszLibToInjectFile, cb, nullptr)) __leave;

		// Get the real address of LoadLibraryW in Kernel32.dll
		PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
			GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
		if (pfnThreadRtn == nullptr) __leave;

		// Create a remote thread that calls LoadLibraryW(DLLPathname)
		hThread = CreateRemoteThread(hProcess, nullptr, 0,
			pfnThreadRtn, pszLibFileRemote, 0, nullptr);
		if (hThread == nullptr) {
			DWORD dwError = GetLastError();
			__leave;
		}

		// Wait for the remote thread to terminate
		WaitForSingleObject(hThread, INFINITE);

		// Resume main thread.
		//
		ResumeThread(pi.hThread);

		bOk = TRUE; // Everything executed successfully
	}
	__finally { // Now, we can clean everything up

				// Free the remote memory that contained the DLL's pathname
		if (pszLibFileRemote != nullptr)
			VirtualFreeEx(hProcess, pszLibFileRemote, 0, MEM_RELEASE);

		if (hThread != nullptr)
			CloseHandle(hThread);

		if (hProcess != nullptr)
			CloseHandle(hProcess);
	}

	return(bOk);
}