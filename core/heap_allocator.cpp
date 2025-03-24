#include "memory.hpp"
#include <stdlib.h>

namespace core {
void* heap_alloc(isize size, isize align){
	ensure(mem_valid_alignment(align), "Invalid alignment");
	align = max<isize>(align, alignof(void*));

	isize space = align - 1 + sizeof(void*) + size;
	void* allocated_mem = calloc(space, 1);

	ensure(allocated_mem != nullptr, "Heap allocation failed");
	void* aligned_mem = reinterpret_cast<void*>(uintptr(allocated_mem) + sizeof(void*));

	uintptr aligned_ptr = 0;
	/* Align the pointer by rounding down */ {
		auto p = uintptr(aligned_mem);
		auto a = uintptr(align);
		aligned_ptr = (p + a - 1) & ~(a - 1);
	};
	aligned_mem = reinterpret_cast<void*>(aligned_ptr);

	/* Store actual memory address before the aligned one */ {
		auto ptr_loc = ((void**)aligned_mem) - 1;
		*ptr_loc = allocated_mem;
	}

	return aligned_mem;
}

void heap_free(void* ptr){
	auto pointer_array = (void**)ptr;
	auto actual_memory = pointer_array[-1];
	free(actual_memory);
}

void* HeapAllocator::alloc(isize size, isize align){
	return heap_alloc(size, align);
}

void* HeapAllocator::realloc(void* old_ptr, isize old_size, isize new_size, isize align){
	// TODO: Perform a proper resize
	auto new_ptr = heap_alloc(new_size, align);
	if(old_ptr == nullptr){ return new_ptr; }

	if(new_ptr == nullptr){ return nullptr; }
	mem_copy_no_overlap(new_ptr, old_ptr, min(new_size, old_size));
	this->free(old_ptr, old_size, align);

	return new_ptr;
}

void HeapAllocator::free(void* ptr, isize, isize){
	heap_free(ptr);
}

void HeapAllocator::free_all(){
	/* Unsupported */
}

// Get the heap allocator handle
HeapAllocator* heap_allocator(){
	static bool init = false;
	static HeapAllocator ha;
	if(!init){
		new (&ha) HeapAllocator();
		init = true;
	}
	return &ha;
}
} /* Universal namespace */
