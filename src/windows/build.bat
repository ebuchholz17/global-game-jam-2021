@echo off
IF NOT EXIST ..\..\build mkdir ..\..\build
IF NOT EXIST ..\..\build\windows mkdir ..\..\build\windows
pushd ..\..\build\windows

REM cl -MT -nologo -Gm- -GR- -EHa- -O2 -Oi -WX -W4 -wd4505 -wd4201 -wd4100 -wd4189 ..\..\src\windows\win_platform.cpp /Fequickmake.exe /link /OPT:REF /SUBSYSTEM:WINDOWS Winmm.lib
cl -MT -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4505 -wd4201 -wd4100 -wd4189 -FC -Z7  ..\..\src\windows\win_platform.cpp /Fequickmake.exe /link /OPT:REF /SUBSYSTEM:WINDOWS Winmm.lib

popd

