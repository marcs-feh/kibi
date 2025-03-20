#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>
#include <stdatomic.h>

//// Basic
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef unsigned int uint;
typedef uint8_t byte;
typedef i32 rune;

typedef ptrdiff_t isize;
typedef uintptr_t uintptr;

typedef float f32;
typedef double f64;

#define Atomic(T) _Atomic(T)

#define max(A, B) (((A) > (B)) ? (A) : (B))
#define min(A, B) (((A) < (B)) ? (A) : (B))
#define clamp(Lo, X, Hi) min(max((Lo), (X)), (Hi))

#define containerof(Ptr, Type, Member) \
	((Type *)(((void *)(Ptr)) - offsetof(Type, Member)))

#define alignofexpr(Expr) (alignof(typeof(Expr)))

#define structmember(Type, MemberName) ((Type){}).MemberName

#define structptrmember(Type, MemberName) ((Type){})->MemberName

#if __STDC_VERSION__ < 202000L
#undef bool
typedef _Bool bool;
#define static_assert(Pred, Msg) _Static_assert(Pred, Msg)
#define typeof(X) __typeof__(X)
#endif

#if !defined(_MSVC_VER)
#define hint_likely(X)   __builtin_expect(!!(X), 1)
#define hint_unlikely(X) __builtin_expect(!!(X), 0)
#else
#define hint_likely(X)   (!!(X))
#define hint_unlikely(X) (!!(X))
#endif

static_assert(sizeof(f32) == 4 && sizeof(f64) == 8, "Bad float size");
static_assert(sizeof(isize) == sizeof(isize), "Mismatched (i/u)size");
static_assert(sizeof(void(*)(void)) == sizeof(void*), "Function pointers and data pointers must be of the same width");
static_assert(sizeof(void(*)(void)) == sizeof(uintptr), "Mismatched pointer types");

extern int printf(const char*, ...);

static inline
_Noreturn void panic(char const* msg) {
	printf("Panic: %s\n", msg);
	__builtin_trap();
}

static inline
void ensure_ex(bool pred, char const* msg, char const* file, i32 line){
	if(hint_unlikely(!pred)){
		printf("(%s:%d) Assertion failed: %s\n", file, line, msg);
		__builtin_trap();
	}
}

#define ensure(Pred, Msg) ensure_ex((Pred), (Msg), __FILE__, __LINE__)

typedef struct String String;

#define str_literal(cslit) (String){ .v = (uint8_t const*)("" cslit ""), .len = (sizeof(cslit) - 1) }

struct String {
	u8 const * v;
	isize len;
};

//// Memory

static inline
bool mem_valid_alignment(isize align){
	return ((align & (align - 1)) == 0) && (align > 0);
}

static inline
uintptr mem_align_forward_ptr(uintptr p, isize a){
	ensure(mem_valid_alignment(a), "Invalid memory alignment");
	uintptr mod = p & (a - 1);
	if(mod != 0){
		p += a - mod;
	}
	return p;
}

static inline
isize mem_align_forward_size(isize p, isize a){
	ensure(mem_valid_alignment(a), "Invalid memory alignment");
	uintptr mod = p & (a - 1);
	if(mod != 0){
		p += a - mod;
	}
	return p;
}

void mem_set(void* dest, u8 val, isize count);

void mem_copy(void* dest, void const* src, isize count);

void mem_copy_no_overlap(void* dest, void const* src, isize count);

i32 mem_compare(void const* lhs, void const* rhs, isize count);

//// Arena
typedef struct Arena Arena;
typedef struct ArenaRegion ArenaRegion;

struct Arena {
	void* data;
	size_t offset;
	size_t capacity;
	void* last_allocation;
	i32 region_count;
};

struct ArenaRegion {
	Arena* arena;
	size_t offset;
};

#define arena_make(A, Type, Count) \
	((Type *)arena_alloc((A), sizeof(Type) * (Count), alignof(Type)))

#define arena_make_array(A, ArrayType, Count) \
	(ArrayType){\
  		.v = arena_alloc((A), (Count) * sizeof(structmember(ArrayType, v)[0]), alignofexpr(structmember(ArrayType, v)[0])), \
  		.len = (Count), \
	}

#define arena_make_string(A, Length) arena_make_array((A), String, (Length))

#define arena_resize_array(A, ArrayPtr, NewSize) do { \
	void* ___tmp_arr_ptr = arena_realloc( \
		/*    Arena */ (A), \
		/*  Pointer */ (void*)((ArrayPtr)->v), \
		/* Old Size */ (ArrayPtr)->len * sizeof((ArrayPtr)->v[0]), \
		/* New size */ (NewSize) * sizeof((ArrayPtr)->v[0]), \
		/*    Align */ alignofexpr((ArrayPtr)->v[0])); \
	if(___tmp_arr_ptr){ \
		(ArrayPtr)->v = ___tmp_arr_ptr; \
		(ArrayPtr)->len = (NewSize); \
	} \
} while(0)

Arena arena_create(byte* buf, isize buf_size);

void* arena_alloc(Arena* arena, isize size, isize align);

bool arena_resize_in_place(Arena* arena, void* ptr, isize size);

void* arena_realloc(Arena* arena, void* ptr, isize old_size, isize new_size, isize align);

void arena_reset(Arena* arena);

ArenaRegion arena_region_begin(Arena* a);

void arena_region_end(ArenaRegion reg);

//// UTF-8
typedef struct UTF8EncodeResult UTF8EncodeResult;
typedef struct UTF8DecodeResult UTF8DecodeResult;
typedef struct UTF8Iterator UTF8Iterator;

struct UTF8EncodeResult {
	u8 bytes[4];
	i8 len;
};

struct UTF8DecodeResult {
	rune codepoint;
	i8 len;
};

struct UTF8Iterator {
	byte const* data;
	isize len;
	isize current;
};

bool utf8_iter_next(UTF8Iterator* iter, UTF8DecodeResult* out);

bool utf8_iter_prev(UTF8Iterator* iter, UTF8DecodeResult* out);

static inline
bool utf8_continuation_byte(rune c){
	static const rune CONTINUATION1 = 0x80;
	static const rune CONTINUATION2 = 0xbf;
	return (c >= CONTINUATION1) && (c <= CONTINUATION2);
}

#define UTF8_ERROR ((rune)(0xfffd))

#define UTF8_ERROR_ENCODED (UTF8EncodeResult){ .bytes = {0xef, 0xbf, 0xbd}, .len = 0 }

