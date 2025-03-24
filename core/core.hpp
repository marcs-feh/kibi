#pragma once
#include <stdint.h>
#include <stddef.h>
#include <new>

#if defined(__clang__) || defined(__GNUC__)
	#define forceinline __attribute__((always_inline)) inline
#elif defined(_MSVC_VER)
	#define forceinline __forceinline
#else
	#error "Unsupported compiler, could not find forceinline builtin"
#endif

//// Meta-programming and type trait boilerplate
#include "meta.hpp"

namespace core {

//// Primitive types
using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using f32 = float;
using f64 = double;

using usize = size_t;
using isize = ptrdiff_t;

using rune = i32;
using byte = u8;
using uintptr = uintptr_t;

static_assert(sizeof(usize) == sizeof(isize), "Mismatched size types");
static_assert(sizeof(f32) == 4 && sizeof(f64) == 8, "Bad float sizes");
static_assert(sizeof(void*) == sizeof(void(*)(void*)), "Mismatched data and function pointers");

template<typename T>
T min(T a, T b){
	return a < b ? a : b;
}

template<typename T, typename... Rest>
T min(T a, T b, Rest&&... rest){
	T x = a < b ? a : b;
	return min(x, rest...);
}

template<typename T>
T max(T a, T b){
	return a > b ? a : b;
}

template<typename T, typename... Rest>
T max(T a, T b, Rest&&... rest){
	T x = a > b ? a : b;
	return max(x, rest...);
}

template<typename T>
T clamp(T lo, T x, T hi){
	return min(max(lo, x), hi);
}

extern "C" { int printf(const char* fmt, ...); }

[[noreturn]] static inline
void panic(char const* msg){
	printf("Panic: %s\n", msg);
	__builtin_trap();
}

static inline
void ensure(bool pred, char const* msg){
	[[unlikely]] if(!pred){
		printf("Failed assertion: %s\n", msg);
		__builtin_trap();
	}
}

template<typename T> forceinline
constexpr RemoveReference<T>&& move(T&& v){
	return static_cast<RemoveReference<T>&&>(v);
}

template<typename T> forceinline
constexpr T&& forward(RemoveReference<T>&& arg) {
	return static_cast<T&&>(arg);
}

template<typename T> forceinline
constexpr T&& forward(RemoveReference<T>& arg) {
	return static_cast<T&&>(arg);
}

template<typename T, typename U = T> forceinline
constexpr T exchange(T& obj, U&& new_val){
	T old_val = core::move(obj);
	obj = forward<U>(new_val);
	return old_val;
}

//// Defer
namespace defer_implementation {
template<typename F>
struct Deferred {
	F f;
	explicit Deferred(F&& f) : f(static_cast<F&&>(f)){}
	~Deferred(){ f(); }
};
template<typename F>
auto make_deferred(F&& f){
	return Deferred<F>(static_cast<F&&>(f));
}

#define _impl_defer_concat0(X, Y) X##Y
#define _impl_defer_concat1(X, Y) _impl_defer_concat0(X, Y)
#define _impl_defer_concat_counter(X) _impl_defer_concat1(X, __COUNTER__)
#define defer(Stmt) auto _impl_defer_concat_counter(_defer_) = ::defer_implementation::make_deferred([&](){ do { Stmt ; } while(0); return; })
}

static inline
void ensure_bounds_check(bool pred, char const* msg){
	#ifndef DISABLE_BOUNDS_CHECK
	[[unlikely]] if(!pred){
		printf("Bounds check failed: %s\n", msg);
		__builtin_trap();
	}
	#else
	(void)pred; (void)msg;
	#endif
}

template<typename T>
struct Maybe {
	T unwrap(){
		if(has_value_){
			T val = core::move(value_);
			has_value_ = false;
			return val;
		}
		panic("Attempt to unwrap() empty maybe");
	}

	template<typename U>
	T or_else(U&& alt){
		if(has_value_){
			T val = core::move(value_);
			return val;
		}
		return forward<U>(alt);
	}

	Maybe<T> copy()
		requires CopyConstructible<T>
	{
		if(!has_value_){
			return {};
		}
		return value_;
	}

	Maybe<T>* drop(){
		if constexpr (!TriviallyDestructible<T>){
			if(has_value_){
				value_.~T();
			}
		}
		has_value_ = false;
		return this;
	}

	[[nodiscard]] constexpr forceinline auto ok() const { return has_value_; }

	constexpr Maybe() : value_{}, has_value_{false} {}

	constexpr Maybe(T const& val) : value_{val}, has_value_{true} {}

	constexpr Maybe(T&& val) : value_{core::move(val)}, has_value_{true} {}

	constexpr Maybe(Maybe<T> const&) = delete;

	constexpr Maybe& operator=(T&& value){
		return *new(this->drop()) Maybe{core::move(value)};
	}

	constexpr Maybe& operator=(Maybe&& value){
		return *new(this->drop()) Maybe{core::move(value)};
	}

private:
	T value_;
	bool has_value_;
};

template<typename V, typename E>
	requires DefaultInitalizable<E> && CopyConstructible<E> && TriviallyDestructible<E>
struct Result {
	V unwrap(){
		if(has_value_){
			auto val = core::move(value_);
			has_value_ = false;
			return val;
		}
		panic("Attempt to unwrap() error result");
	}

	E unwrap_error(){
		if(!has_value_){
			return error_;
		}
		panic("Attempt to unwrap_error() value result");
	}

	template<typename U>
	V or_else(U&& alt){
		if(has_value_){
			auto val = core::move(value_);
			has_value_ = false;
			return val;
		}
		return forward<U>(alt);
	}

	Result* drop(){
		if constexpr(!TriviallyDestructible<V>){
			if(has_value_){
				value_.~V();
			}
		}
		has_value_ = false;
		return this;
	}

	Result copy(){
		if(!has_value_){
			return Result { error_ };
		}
		return Result { value_ };
	}

	[[nodiscard]] constexpr forceinline auto ok() const { return has_value_; }

	constexpr Result() : error_{}, has_value_{false} {}
	constexpr Result(E const& error) : error_{error}, has_value_{false} {}
	constexpr Result(V&& value) : value_{core::move(value)}, has_value_{true} {}
	constexpr Result(V const& value) : value_{value}, has_value_{true} {}

	constexpr Result& operator=(E const& err){
		return *new (this->drop()) Result{ err };
	}

	constexpr Result& operator=(V&& value){
		return *new (this->drop()) Result{ core::move(value) };
	}
	constexpr Result& operator=(Result&& value){
		return *new (this->drop()) Result{ core::move(value) };
	}

	~Result(){ this->drop(); }
private:
	union {
		V value_;
		E error_;
	};
	bool has_value_;
};

template<typename T>
struct LinearIterator {
	T* data;

	T& operator*(){ return *data; }
	T* operator->() { return data; }
	void operator++(){ data++; }

	bool operator==(LinearIterator const& it){ return it.data == data; }
	bool operator!=(LinearIterator const& it){ return it.data != data; }
};

template<typename T>
struct Slice {
	T& operator[](isize idx){
		ensure_bounds_check(idx >= 0 && idx < len_, "Index to slice is out of bounds");
		return data_[idx];
	}

	T const& operator[](isize idx) const{
		ensure_bounds_check(idx >= 0 && idx < len_, "Index to slice is out of bounds");
		return data_[idx];
	}

	Maybe<T> get(isize idx){
		if(idx >= 0 && idx < len_){
			return data_[idx];
		}
		return Maybe<T>();
	}

	bool set(isize idx, T val){
		if(idx >= 0 && idx < len_){
			data_[idx] = val;
			return true;
		}
		return false;
	}

	// Getters
	[[nodiscard]] constexpr forceinline auto len() const { return len_; }
	[[nodiscard]] constexpr forceinline auto data() const { return data_; }

	constexpr Slice() : data_{0}, len_{0}{}
	constexpr Slice(T* data, isize length) : data_{data}, len_{length} {}

	// C++ Iterator insanity
	constexpr auto begin(){ return LinearIterator{ data_ }; }
	constexpr auto end(){ return LinearIterator{ data_ + len_ }; }

private:
	T*    data_ {0};
	isize len_ {0};
};

template<typename To, typename From>
constexpr forceinline
To bit_cast(From&& v){
	return __builtin_bit_cast(To, forward<From>(v));
}

static inline constexpr
isize cstring_len(const char* p){
	isize i = 0;
	for(i = 0; p[i] != 0; i++){}
	return i;
}

struct UTF8Encode {
	byte bytes[4];
	u8  len;
};

struct UTF8Decode {
	rune codepoint;
	u8  len;
};

UTF8Encode utf8_encode(rune c);

UTF8Decode utf8_decode(byte const* buf, isize len);

struct String {
	byte operator[](isize idx) const {
		ensure_bounds_check(idx >= 0 && idx < len_, "Index to string is out of bounds");
		return data_[idx];
	}

	static String from_bytes(Slice<byte> buf){
		String s;
		s.data_ = buf.data();
		s.len_ = buf.len();
		return s;
	}

	constexpr String(){}

	constexpr String(char const* p)
		: data_{bit_cast<byte const*>(p)}
		, len_{cstring_len(p)} {}

	// Getters
	[[nodiscard]] constexpr forceinline auto len() const { return len_; }
	[[nodiscard]] constexpr forceinline auto data() const { return data_; }

private:
	byte const* data_ {0};
	isize len_ {0};
};

} /* Universal namespace */

