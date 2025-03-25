@echo off

REM clang Build version (recommended)
clang -Os -std=c++20 -Wall -Wextra -fno-strict-aliasing -fwrapv -o main.exe main.cpp core\core.cpp kielo.cpp
if %errorlevel% neq 0 exit /b %errorlevel%

REM cl Build version
REM cl /nologo /std:c++20 /Os /EHsc /GR /W4 /Femain.exe main.cpp core\core.cpp kielo.cpp
if %errorlevel% neq 0 exit /b %errorlevel%

