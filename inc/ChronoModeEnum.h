#define CHRONO_MODE_ARG L"CHRONO_MODE_ARG"

enum ChronoModeEnum
{
	TimeOffset = 0,
	TimeFlowRatio,

	LastVal
};

LPCWSTR ChronoModeDesc[]
{
	L"ChronomorphTimeOffset",
	L"ChronomorphTimeFlowRatio",
};

#define CHRONO_MODE_ARG_1 L"CHRONO_MODE_ARG_1"
#define CHRONO_MODE_ARG_2 L"CHRONO_MODE_ARG_2"