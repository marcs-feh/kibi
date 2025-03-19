#include "base.h"

// By default, use a stricter alignment
#define ARENA_MIN_ALIGNMENT ((isize)(alignof(void*) * 2))

static inline
uintptr arena_align_forward_ptr(uintptr p, uintptr a){
	ensure(mem_valid_alignment(a), "Alignment must be a power of 2 greater than 0");
	uintptr mod = p & (a - 1); /* Fast modulo for powers of 2 */
	if(mod > 0){
		p += (a - mod);
	}
	return p;
}

Arena arena_create(byte* buf, isize buf_size){
	return (Arena){
		.data = buf,
		.offset = 0,
		.capacity = buf_size,
		.last_allocation = NULL,
		.region_count = 0,
		.last_error = 0,
	};
}

void* arena_alloc(Arena* a, isize size, isize align){
	align = max(ARENA_MIN_ALIGNMENT, align);
	uintptr base = (uintptr)a->data;
	uintptr current = base + (uintptr)a->offset;

	isize available = a->capacity - (current - base);

	uintptr aligned  = mem_align_forward_ptr(current, align);
	uintptr padding  = aligned - current;
	isize required = padding + size;

	if(required > available){
		return NULL; /* Out of memory */
	}

	a->offset += required;
	void* allocation = (void*)aligned;
	a->last_allocation = allocation;
	__builtin_memset(allocation, 0, size);

	return allocation;
}

bool arena_resize(Arena* a, void* ptr, isize new_size){
	uintptr base    = (uintptr)a->data;
	uintptr current = base + (uintptr)a->offset;
	uintptr limit   = base + a->capacity;

	ensure((uintptr)ptr >= base && (uintptr)ptr < limit, "Pointer is not owned by arena");

	if(ptr == a->last_allocation){
		isize last_allocation_size = current - (uintptr)a->last_allocation;
		if(((current - last_allocation_size) + new_size) > limit){
			return false; /* No space left */
		}

		a->offset += new_size - last_allocation_size;
		return true;
	}

	return false;
}

void arena_reset(Arena* arena){
	ensure(arena->region_count == 0, "Arena has dangling regions");
	arena->offset = 0;
	arena->last_allocation = NULL;
}

ArenaRegion arena_region_begin(Arena* a){
	ArenaRegion reg = {
		.arena = a,
		.offset = a->offset,
	};
	a->region_count += 1;
	return reg;
}

void arena_region_end(ArenaRegion reg){
	ensure(reg.arena->region_count > 0, "Arena has a improper region counter");
	ensure(reg.arena->offset >= reg.offset, "Arena has a lower offset than region");

	reg.arena->offset = reg.offset;
	reg.arena->region_count -= 1;
}

static
void* arena_allocator_func(
	void* impl,
	AllocatorMode mode,
	isize new_size,
	isize new_align,
	void* old_ptr,
	isize old_size,
	isize old_align
){
	Arena* a = (Arena*)impl;

	void* result = NULL;
	AllocatorError error = 0;

	(void)old_align;

	switch(mode){
	case AllocatorMode_Alloc: {
		result = arena_alloc(a, new_size, new_align);
		if(result == NULL) {
			error = AllocatorError_OutOfMemory;
		}
	} break;

	case AllocatorMode_Realloc: {
		bool ok = arena_resize(a, old_ptr, new_size);
		if(!ok){
			result = arena_alloc(a, new_size, new_align);
			if(result != NULL){
				mem_copy_no_overlap(result, old_ptr, min(old_size, new_size));
			}
			else {
				error = AllocatorError_OutOfMemory;
			}
		}
		else {
			result = old_ptr; /* Resize in-place successful */
		}

		/* Pad positive excess with zeros */
		if(!error && new_size > old_size){
			byte* buf = result;
			isize delta = new_size - old_size;
			mem_set(&buf[old_size], 0, delta);
		}
	} break;

	case AllocatorMode_Free: {
		error = AllocatorError_Unsupported;
	} break;

	case AllocatorMode_FreeAll: {
		arena_reset(a);
	} break;

	case AllocatorMode_GetError: {
		uintptr err = (uintptr)a->last_error;
		result = (void*)err;
	} break;

	case AllocatorMode_Query: {
	} break;

	default: {
		a->last_error = AllocatorError_UnknownMode;
	} break;
	}

	a->last_error = error;
	return result;
}

Allocator arena_allocator(Arena* a){
	return (Allocator){
		.data = (void*)a,
		.func = arena_allocator_func,
	};
}


