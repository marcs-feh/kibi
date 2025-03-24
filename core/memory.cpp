#include "core.hpp"

// TODO: Support MSVC

namespace core {
void mem_set(void* p, byte val, isize count){
	__builtin_memset(p, val, count);
}

void mem_copy(void* dest, void const* source, isize count){
	__builtin_memmove(dest, source, count);
}

void mem_copy_no_overlap(void* dest, void const* source, isize count){
	__builtin_memcpy(dest, source, count);
}

i32 mem_comapre(void const* lhs, void const* rhs, isize count){
	return __builtin_memcmp(lhs, rhs, count);
}
} /* Universal namespace */
