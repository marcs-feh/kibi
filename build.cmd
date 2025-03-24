@echo off

REM For a cl build use this instead:
cl /nologo /std:c++20 /Os /EHsc /GR /W4 /Femain.exe main.cpp core\core.cpp 

REM clang -Os -std=c++20 -Wall -Wextra -fno-strict-aliasing -fwrapv -o main.exe main.cpp core\core.cpp 

