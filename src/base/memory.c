#include "base.h"

void mem_set(void* dest, u8 val, isize count){
	__builtin_memset(dest, val, count);
}

void mem_copy(void* dest, void const* src, isize count){
	__builtin_memmove(dest, src, count);
}

void mem_copy_no_overlap(void* dest, void const* src, isize count){
	__builtin_memcpy(dest, src, count);
}

i32 mem_compare(void const* lhs, void const* rhs, isize count){
	return __builtin_memcmp(lhs, rhs, count);
}


