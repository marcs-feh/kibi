#include <stdio.h>
#include "base/base.h"
#include "mimalloc.h"

typedef struct {
	f32* v;
	isize len;
} F32Array;

int main(){
	static byte arena_memory[4 * 1024];
	Arena arena = arena_create(arena_memory, 4 * 1024);
	Allocator allocator = arena_allocator(&arena);

	F32Array p0 = mem_make_array(allocator, F32Array, 237);
	F32Array p1 = mem_make_array(allocator, F32Array, 173);


	printf("%p %zu\n", p0.v, p0.len);
	printf("%p %zu\n", p1.v, p1.len);
	printf("%zu\n", arena.offset);

	// mem_delete(allocator, p0);

	// void* p1 = mem_alloc(allocator, 1, 1);
	// printf("%zu\n", arena.offset);

	// p0 = mem_realloc(allocator, p0, 200, 64, 600, 64);
	// printf("%zu\n", arena.offset);

	return 0;
}
