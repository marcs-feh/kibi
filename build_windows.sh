#!/usr/bin/env sh

buildMode="$1"
[ -z "$buildMode" ] && buildMode='debug'

cc='clang -std=c17'
cflags='-Wall -Wextra -fno-strict-aliasing' # -fPIC -static-libgcc'
iflags='-I./base -I./deps/include'
ldflags='-luser32 -lkernel32 -ladvapi32'
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
	local cc='clang -std=c17 -Wall -Wextra'
	local ignoreflags='-Wno-pointer-sign'
	local cflags='-O3 -fno-strict-aliasing -DNDEBUG'

	cd ./deps
	Run $cc $cflags $ignoreflags mimalloc/src/static.c -I mimalloc/include -c -o mimalloc/mimalloc.o
	cd ..
}

cflags="$cflags $iflags"

set -eu

# Build mimalloc if needed
[ -f "deps/mimalloc/mimalloc.o" ] || BuildMimalloc

Run $cc $cflags -o test.exe \
	deps/mimalloc/mimalloc.o src/main.c \
	$ldflags


