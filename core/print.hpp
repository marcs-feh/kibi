#pragma once

#include "core.hpp"
#include "format.hpp"
#include "string_builder.hpp"

namespace core {
constexpr isize print_arena_size = 2 * 1024 * 1024;

Arena* print_arena_get();

template<typename T, typename ...Args>
	requires(Formattable<T>)
void print(T const& val, Args const&... args){
	auto reg = print_arena_get()->create_region();
	defer(reg.release());
	FormattingContext ctx;
	ctx.buffer = StringBuilder::create(print_arena_get(), 128);

	print_impl(ctx, args...);
}


template<typename T>
	requires(Formattable<T>)
void print_impl(FormattingContext* ctx, T const& v){
	format(ctx, v);
	auto result = ctx->buffer.build();
	printf("%.*s\n", (char*)result.data(), (int)result.len());
}

template<typename T, typename ...Args>
	requires(Formattable<T>)
void print_impl(FormattingContext* ctx, T const& v, Args const&... args){
	format(ctx, v);
	format(ctx, args...);
}

}

