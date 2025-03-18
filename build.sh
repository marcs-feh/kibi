#!/usr/bin/env sh

buildMode="$1"
[ -z "$buildMode" ] && buildMode='debug'

cc='clang -std=c17'
cflags='-Wall -Wextra -fPIC -fno-strict-aliasing -static-libgcc'
iflags='-I./base -I./deps/include'
ldflags=''
ar='llvm-ar'

case "$buildMode" in
	'debug')
		cflags="$cflags -g -O0 -fsanitize=address"
	;;
	'release')
		cflags="$cflags -s -O3"
	;;
esac

Run(){ echo "* $@"; $@; }

BuildMimalloc(){
	local cc='clang -std=c17'
	local cflags='-O3 -fPIC -fno-strict-aliasing -DNDEBUG'

	cd ./deps
	Run $cc $cflags mimalloc/src/static.c -I mimalloc/include -c -o mimalloc/mimalloc.o
	cd ..
}

set -eu

# Build mimalloc if needed
[ -f "deps/mimalloc/mimalloc.o" ] || BuildMimalloc

Run $cc $cflags -o test.exe \
	deps/mimalloc/mimalloc.o src/main.c \
	$ldflags

