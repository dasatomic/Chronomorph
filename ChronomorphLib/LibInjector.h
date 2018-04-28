#pragma once

#include "stdafx.h"
#include <Windows.h>

bool WINAPI InjectLibAtStartup(PWSTR pszExeToStartPath, PCWSTR pszLibToInjectFile);