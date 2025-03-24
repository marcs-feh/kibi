#pragma once
#include "build_context.hpp"

template<typename T>
struct Identity {
	using Type = T;
};

template<typename T>
concept Referenceable = requires {
	typename Identity<T&>;
};

template<typename A, typename B>
constexpr inline bool is_same_type = false;

template<typename A>
constexpr inline bool is_same_type<A, A> = true;

template<typename A, typename B>
concept SameAs = is_same_type<A, B>;

template<typename T, typename ...Args>
concept Constructible =
	#if defined(COMPILER_CLANG) || defined(COMPILER_GCC) || defined(COMPILER_MSVC)
		__is_constructible(T, Args...)
	#else
		#error "Compiler not supported, could not find a builtin to detect if type is constructible"
	#endif
;

template<typename T>
concept TriviallyConstructible =
		 __is_trivially_constructible(T)
	// #elif __has_builtin(__has_trivial_constructor)
	// 	__has_trivial_constructor(T)
	// #else
	// 	#error "Compiler not supported, could not find a builtin to detect if type is trivially constructible"
	// #endif
;

template<typename T>
concept TriviallyDestructible =
	// #if __has_builtin(__is_trivially_destructible)
			__is_trivially_destructible(T);
	// #elif __has_builtin(__has_trivial_destructor)
		// __has_trivial_destructor(T);
	// #else
	// 	#error "Compiler not supported, could not find a builtin to detect if type is trivially destructible"
	// #endif
;

template<typename T>
concept TriviallyCopyable =
	// #if __has_builtin(__is_trivially_copyable)
		__is_trivially_copyable(T)
	// #else
	// 	#error "Compiler not supported, could not find a builtin to detect if type is trivially copyable"
	// #endif
;

template<typename T>
concept Destructible = requires(T obj){
	obj.~T();
};

template<typename T, typename ...Args>
concept ConstructibleFrom = Destructible<T> && Constructible<T, Args...>;

template<typename T>
concept DefaultInitalizable = requires {
	T{};
	::new T;
} && ConstructibleFrom<T>;

template<typename From, typename To>
concept ConvertibleTo = requires(From v) {
	static_cast<To>(v);
};

namespace meta_implementation {
template<typename T> struct RemoveReference_{ using Type = T; };
template<typename T> struct RemoveReference_<T&>{ using Type = T; };
template<typename T> struct RemoveReference_<T&&>{ using Type = T; };
}

template<typename T>
using RemoveReference = typename meta_implementation::RemoveReference_<T>::Type;

// NOTE: Non Referenceable types are not affected by "Add X reference", so a the
// referencing version is provided as an specialization
namespace meta_implementation {
template<typename T, auto can_ref = Referenceable<T>>
struct AddLValueReference_ { using Type = T; };

template<typename T>
struct AddLValueReference_<T, true> { using Type = T&; };

template<typename T, auto can_ref = Referenceable<T>>
struct AddRValueReference_ { using Type = T; };

template<typename T>
struct AddRValueReference_<T, true> { using Type = T&&; };
}

template<typename T>
using AddLValueReference = typename meta_implementation::AddLValueReference_<T>::Type;

template<typename T>
using AddRValueReference = typename meta_implementation::AddRValueReference_<T>::Type;

// Constructability
template<typename T>
concept CopyConstructible = Constructible<T, AddLValueReference<const T>>;

template<typename T>
concept MoveConstructible = Constructible<T, AddRValueReference<T>>;

