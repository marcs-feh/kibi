#include "base/base.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	f32* v;
	isize len;
} F32Array;

int main(){
	Arena arena = arena_create(malloc(4 * 1024), 4 * 1024);

	F32Array arr = arena_make_array(&arena, F32Array, 100);
	printf("%p : %zu\n", arr.v, arr.len);
	arena_resize_array(&arena, &arr, 300);
	printf("%p : %zu\n", arr.v, arr.len);

	arena_reset(&arena);
	free(arena.data);
	return 0;
}
