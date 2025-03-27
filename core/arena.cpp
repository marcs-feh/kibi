#include "memory.hpp"

namespace core {

Arena Arena::create(Slice<byte> buf){
	Arena a;
	a.data = buf.data();
	a.offset = 0;
	a.capacity = buf.len();
	a.last_allocation = nullptr;
	a.region_count = 0;
	return a;
}

void* Arena::alloc(isize size, isize align){
	uintptr base = (uintptr)this->data;
	uintptr current = base + (uintptr)this->offset;

	isize available = this->capacity - isize(current - base);

	uintptr aligned = mem_align_forward_ptr(current, align);
	isize padding   = aligned - current;
	isize required  = padding + size;

	if(required > available){
		return nullptr; /* Out of memory */
	}

	this->offset += required;
	void* allocation = (void*)aligned;
	this->last_allocation = allocation;
	mem_set(allocation, 0, size);

	return allocation;
}

void* Arena::realloc(void* old_ptr, isize old_size, isize new_size, isize align){
	if(old_ptr == nullptr){
		return this->alloc(new_size, align);
	}
	bool ok = this->resize_in_place(old_ptr, new_size);

	if(ok){
		return old_ptr;
	}
	else {
		void* p = this->alloc(new_size, align);
		if(p != nullptr && old_ptr != nullptr){
			mem_copy_no_overlap(p, old_ptr, min(old_size, new_size));
		}
		return p;
	}
}

bool Arena::resize_in_place(void* ptr, isize new_size){
	uintptr base    = (uintptr)this->data;
	uintptr current = base + (uintptr)this->offset;
	uintptr limit   = base + this->capacity;

	ensure((uintptr)ptr >= base && (uintptr)ptr < limit, "Pointer is not owned by arena");

	if(ptr == this->last_allocation && ptr != nullptr){
		isize last_allocation_size = current - (uintptr)this->last_allocation;
		if(((current - last_allocation_size) + new_size) > limit){
			return false; /* No space left */
		}

		this->offset += new_size - last_allocation_size;
		return true;
	}

	return false;
}

void Arena::free(void*, isize, isize){
	/* Unsupported */
}

void Arena::free_all(){
	ensure(this->region_count == 0, "Arena has dangling regions");
	this->offset = 0;
	this->last_allocation = nullptr;
}

ArenaRegion ArenaRegion::create(Arena* arena){
	ArenaRegion reg;
	reg.arena = arena;
	reg.offset = arena->offset;
	return reg;
}

ArenaRegion Arena::create_region(){
	return ArenaRegion::create(this);
}

void ArenaRegion::release(){
	auto a = this->arena;
	ensure(a->region_count > 0, "Arena has a improper region counter");
	ensure(a->offset >= this->offset, "Arena has a lower offset than region");

	a->offset = this->offset;
	a->region_count -= 1;
}

} /* Universal namespace */
