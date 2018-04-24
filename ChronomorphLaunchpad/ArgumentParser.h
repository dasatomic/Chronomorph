#pragma once

#include <string>
#include <vector>
#include <algorithm>

struct ArgumentBase
{
	std::wstring argumentName;
	bool optional;
	bool valueProvided = false;

	ArgumentBase(std::wstring argumentName, bool optional)
		: argumentName(argumentName), optional(optional) {}

};

template<typename ArgType>
struct Argument : ArgumentBase
{
	ArgType providedValue;

	Argument(std::wstring argumentName, bool optional)
		: argumentName(argumentName), optional(optional) {}
};

std::vector<ArgumentBase*> argumentList = {
	new Argument<std::wstring>(L"-executableToMorph", false),
	new Argument<std::wstring>(L"-timeZoneToInject", true),
};

class ArgumentParser
{
public:
	ArgumentParser(int argc, wchar_t* argv[]) :
		exeToMorphPath(L"-executableToMorph", false),
		timeZoneToInject(L"-timeZoneToInject", true)
	{
		for (int i = 1; i < argc; i++)
		{
			std::wstring argRaw = argv[i];

			// auto argBase = std::find_if(
			// 	argumentList.begin(),
			// 	argumentList.end(),
			// 	[&argRaw](ArgumentBase currArg) { return currArg.argumentName == argRaw; });
		}
	}

	Argument<std::wstring> exeToMorphPath;
	Argument<std::wstring> timeZoneToInject;

};