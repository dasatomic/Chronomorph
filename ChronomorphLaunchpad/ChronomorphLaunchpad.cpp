#include "stdafx.h"
#include "CmdLine.h"

using namespace TCLAP;
using namespace std;

int main(int argc, char *argv[])
{
	CmdLine cmd("Time morpher", ' ' /* argument separator */);

	ValueArg<string> pathToExecutable("e", "executable", "Full path to the executable to me morphed", true, "Param is mandatory", "string");
	cmd.add(pathToExecutable);

	ValueArg<string> timeZoneToInject ("t", "timezone", "Name of timezone to use.", false, "UTC", "string");
	cmd.add(timeZoneToInject);

	ValueArg<int> timeOffset("o", "timeOffset", "Time offset in seconds from current time.", false, 0, "int");
	cmd.add(timeZoneToInject);

	cmd.parse(argc, argv);

    return 0;
}