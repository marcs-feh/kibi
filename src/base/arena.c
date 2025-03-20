#include "base.h"

// By default, use a stricter alignment
#define ARENA_MIN_ALIGNMENT ((isize)(alignof(void*) * 2))

Arena arena_create(byte* buf, isize buf_size){
	return (Arena){
		.data = buf,
		.offset = 0,
		.capacity = buf_size,
		.last_allocation = NULL,
		.region_count = 0,
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

void* arena_realloc(Arena* arena, void* ptr, isize old_size, isize new_size, isize align){
	bool ok = arena_resize_in_place(arena, ptr, new_size);
	isize size_diff = new_size - old_size;
	void* new_alloc = NULL;

	if(ok){
		new_alloc = ptr;
	}
	else {
		new_alloc = arena_alloc(arena, new_size, align);
		mem_copy_no_overlap(new_alloc, ptr, min(old_size, new_size));
	}

	if(new_alloc && size_diff > 0){
		mem_set(&new_alloc[old_size], 0, size_diff);
	}

	return new_alloc;
}

bool arena_resize_in_place(Arena* a, void* ptr, isize new_size){
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

