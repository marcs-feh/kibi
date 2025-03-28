#!/usr/bin/env sh

set -xeu

clang++ -Os -std=c++20 -o main.exe \
	-fwrapv \
	-fno-exceptions \
	-fno-strict-aliasing \
	-fsanitize=address \
	-Wall -Wextra \
	-static-libgcc \
	main.cpp kielo.cpp core/core.cpp

