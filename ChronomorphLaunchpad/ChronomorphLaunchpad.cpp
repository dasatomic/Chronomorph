#include "stdafx.h"
#include "CmdLine.h"
#include "LibInjector.h"
#include <Strsafe.h>

#define CHRONOMORPH_LIB L"ChronomorphLib"

using namespace TCLAP;
using namespace std;


void InjectTimeOffset(string& pathToExecutable, int timeOffset)
{
	std::wstring pathToExecutableWide = std::wstring(pathToExecutable.begin(), pathToExecutable.end());
	
	// Since c_str points to the const string and LibInjector requires non const we need to reallocate it in new buffer.
	//
	wchar_t szExeFile[MAX_PATH];
	StringCchCopy(szExeFile, MAX_PATH, pathToExecutableWide.c_str());

	wchar_t szDllToInject[] = CHRONOMORPH_LIB;

	InjectLibAtStartup(szExeFile, szDllToInject);
}

int main(int argc, char *argv[])
{
	CmdLine cmd("Time morpher", ' ' /* argument separator */);

	ValueArg<string> pathToExecutable("e", "executable", "Full path to the executable to me morphed", true, "Param is mandatory", "string");
	cmd.add(pathToExecutable);

	ValueArg<string> timeZoneToInject ("t", "timezone", "Name of timezone to use.", false, "UTC", "string");
	cmd.add(timeZoneToInject);

	ValueArg<int> timeOffset("o", "timeOffset", "Time offset in seconds from current time.", false, 0, "int");
	cmd.add(timeOffset);

	cmd.parse(argc, argv);

	if (timeOffset.isSet())
	{
		InjectTimeOffset(pathToExecutable.getValue(), timeOffset.getValue());
	}

    return 0;
}