#include "core.hpp"

// TODO: Support MSVC
#if defined(COMPILER_MSVC)
#include <string.h>

namespace core {
void mem_set(void* p, byte val, isize count){
	memset(p, val, size_t(count));
}

void mem_copy(void* dest, void const* source, isize count){
	memmove(dest, source, size_t(count));
}

void mem_copy_no_overlap(void* dest, void const* source, isize count){
	memcpy(dest, source, size_t(count));
}

i32 mem_comapre(void const* lhs, void const* rhs, isize count){
	return memcmp(lhs, rhs, size_t(count));
}
} /* Universal namespace */

#else
namespace core {
void mem_set(void* p, byte val, isize count){
	__builtin_memset(p, val, usize(count));
}

void mem_copy(void* dest, void const* source, isize count){
	__builtin_memmove(dest, source, usize(count));
}

void mem_copy_no_overlap(void* dest, void const* source, isize count){
	__builtin_memcpy(dest, source, usize(count));
}

i32 mem_comapre(void const* lhs, void const* rhs, isize count){
	return __builtin_memcmp(lhs, rhs, usize(count));
}
} /* Universal namespace */
#endif

