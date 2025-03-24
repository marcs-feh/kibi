#pragma once
#include "core.hpp"

namespace core {

//// Memory
void mem_set(void* p, byte val, isize count);

void mem_copy(void* dest, void const* source, isize count);

void mem_copy_no_overlap(void* dest, void const* source, isize count);

i32 mem_comapre(void const* lhs, void const* rhs, isize count);

static inline
bool mem_valid_alignment(isize align){
	return align && ((align & (align - 1)) == 0);
}

static inline
uintptr mem_align_forward_ptr(uintptr p, uintptr a){
	ensure(mem_valid_alignment(a), "Alignment must be a power of 2 greater than 0");
	uintptr mod = p & (a - 1); /* Fast modulo for powers of 2 */
	if(mod > 0){
		p += (a - mod);
	}
	return p;
}

struct Allocator {
	[[nodiscard]]
	virtual void* alloc(isize size, isize align) = 0;
	[[nodiscard]]
	virtual void* realloc(void* old_ptr, isize old_size, isize new_size, isize align) = 0;
	virtual void free(void* ptr, isize size, isize align) = 0;
	virtual void free_all() = 0;

	template<DefaultInitalizable T> [[nodiscard]]
	T* make(){
		auto p = (T*)alloc(sizeof(T), alignof(T));
		[[unlikely]] if(p == nullptr){ return nullptr; }

		new (p) T();
		return p;
	}

	template<DefaultInitalizable T>
	[[nodiscard]] Slice<T> make(isize n){
		auto p = (T*)alloc(sizeof(T) * n, alignof(T));
		[[unlikely]] if(p == nullptr){ return Slice<T>(); }

		auto s = Slice<T>(p, n);
		if constexpr(!TriviallyConstructible<T>){
			for(isize i = 0; i < n; i += 1){
				new (p) T();
			}
		}
		return s;
	}

	template<typename T>
	void drop(T* p){
		if constexpr(!TriviallyDestructible<T>){
			p->~T();
		}
		this->free(p, sizeof(T), alignof(T));
	}

	template<typename T>
	void drop(Slice<T> s){
		auto p = s.data();
		if constexpr(!TriviallyDestructible<T>){
			for(isize i = 0; i < s.len(); i += 1){
				p[i].~T();
			}
		}
		this->free((void*)p, sizeof(T) * s.len(), alignof(T));
	}
};

//// Arena
struct Arena;

struct ArenaRegion {
	Arena* arena;
	isize offset;

	static ArenaRegion create(Arena* arena);
	void release();
};

struct Arena : Allocator {
	void* data;
	isize offset;
	isize capacity;
	void* last_allocation;
	int   region_count;

	ArenaRegion create_region();

	bool resize_in_place(void* ptr, isize new_size);

	void* alloc(isize size, isize align) override;

	void* realloc(void* old_ptr, isize old_size, isize new_size, isize align) override;

	void free(void* ptr, isize size, isize align) override;

	void free_all() override;

	static Arena create(Slice<byte> buf);
};

//// Heap allocator
void* heap_alloc(isize size, isize align);

void heap_free(void* ptr);

struct HeapAllocator : Allocator {
	void* alloc(isize size, isize align) override;

	void* realloc(void* old_ptr, isize old_size, isize new_size, isize align) override;

	void free(void* ptr, isize size, isize align) override;

	void free_all() override;
};

// Get the heap allocator handle
HeapAllocator* heap_allocator();

} /* Universal namespace */
namespace U {
} /* Universal namespace */
