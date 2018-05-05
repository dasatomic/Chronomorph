# Chronomorph
Set of utilities for chaning time flow in targeted process.

Currently supported features are:

--timeFlowRatio - Time will flow at different rate in the child process. E.g. try starting a multimedia player in this mode (tested with VLC and WMP) and video will play at the given speed. You can also start games with this flag and experiance speed up/slowdown.

--timeOffset - Target process will think it is running in different point in time. Try running Word/Wordpad and click on insert date.

Time modificiations are done by mocking a set of time Windows APIs through dll injection (e.g. GetSystemTime, GetLocalTime to start with). Also some slightly more advanced API mocking is done (e.g. scheduling timers at frequency modified by time flow ratio, changing Sleep etc.).


![AppVeoyor](https://ci.appveyor.com/api/projects/status/3aty62v79poucvfo?svg=true)
