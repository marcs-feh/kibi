#pragma once

#if defined(__clang__)
	#define COMPILER_CLANG 1
#elif defined(__GNUC__)
	#define COMPILER_GCC 1
#elif defined(_MSC_VER)
	#define COMPILER_MSVC 1
#else
	#error "Unsupported compiler"
#endif

namespace core {
namespace build_context {

inline constexpr char const compiler_name[] =
	#if defined(COMPILER_CLANG)
		"clang"
	#elif defined(COMPILER_GCC)
		"gcc"
	#elif defined(COMPILER_MSVC)
		"msvc"
	#endif
;

}
}
