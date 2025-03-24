set -eu

clang++ -Os -std=c++20 -o main.exe \
	-fwrapv \
	-fno-exceptions \
	-fno-strict-aliasing \
	-Wall -Wextra \
	main.cpp core/core.cpp

