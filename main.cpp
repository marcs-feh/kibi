#include "core/core.hpp"
#include "core/memory.hpp"

#include <iostream>
#include "lexer.hpp"

using core::String, core::Slice, core::isize;

std::ostream& operator<<(std::ostream& os, String s){
	os.write(s.data(), s.len());
	return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, Slice<T> s){
	for(isize i = 0; i < s.len(); i++)
		os << s[i] << ' ';
	return os;
}

std::ostream& operator<<(std::ostream& os, kielo::Token t){
	os << kielo::token_type_name(t.type) << "\t\"" << t.lexeme << '"';
	return os;
}

#define STB_SPRINTF_IMPLEMENTATION
#include "core/wip/stb_sprintf.h"

using namespace core;


[[nodiscard]]
String format_float(Slice<byte> buf, f64 val, i32 precision = 4, i32 left_pad = 0){
	constexpr char const* float_fmt = "%*.*f";

	isize len = stbsp_snprintf((char*)buf.data(), buf.len(), float_fmt, (int)left_pad, (int)precision, val);

	return String::from_bytes(buf[{0, len}]);
}

[[nodiscard]]
String format_string(Slice<byte> buf, String s, i32 left_pad = 0){
	constexpr char const* str_fmt = "%*.*s";

	isize len = stbsp_snprintf((char*)buf.data(), buf.len(), str_fmt, (int)left_pad, (int)s.len(), s.data());

	return String::from_bytes(buf[{0, len}]);
}

[[nodiscard]]
String format_bool(Slice<byte> buf, bool val){
	char const* fmt = nullptr;
	isize len = 0;

	if(val){
		fmt = "true";
		len = sizeof("true");
	} else {
		fmt = "false";
		len = sizeof("false");
	}

	len = min(buf.len(), len);
	mem_copy(buf.data(), fmt, len);
	return String::from_bytes(buf[{0, len}]);
}

enum class FormattingFlag {
	PadWithZeros       = (1 << 0),
	ScientificNotation = (1 << 1),
	SeparateDigits     = (1 << 2),
};

struct FormattingContext {
	Slice<byte> buffer;
	i32 left_pad;
	i32 base;
	i32 max_width;
	i32 precision;
	u8 flags; /* Bitset of FormattingFlag */
};

[[nodiscard]]
String format(FormattingContext* ctx, i64 const& val){
	bool pad_with_zero = u8(ctx->flags) & u8(FormattingFlag::PadWithZeros);

	constexpr char const* hex_fmt = "%*llx";
	constexpr char const* dec_fmt = "%*lld";
	constexpr char const* oct_fmt = "%*lld";
	constexpr char const* bin_fmt = "%*llb";

	constexpr char const* hex_fmt_z = "%0*llx";
	constexpr char const* dec_fmt_z = "%0*lld";
	constexpr char const* oct_fmt_z = "%0*lld";
	constexpr char const* bin_fmt_z = "%0*llb";
	char const* fmt = nullptr;

	if(pad_with_zero){
		switch (ctx->base) {
			case 16: fmt = hex_fmt_z; break;
			case 10: fmt = dec_fmt_z; break;
			case 8:  fmt = oct_fmt_z; break;
			case 2:  fmt = bin_fmt_z; break;
		}
	}
	else {
		switch (ctx->base) {
			case 16: fmt = hex_fmt; break;
			case 10: fmt = dec_fmt; break;
			case 8:  fmt = oct_fmt; break;
			case 2:  fmt = bin_fmt; break;
		}
	}

	isize len = 0;
	auto buf = ctx->buffer;
	if(fmt != nullptr){
		len = stbsp_snprintf((char*)buf.data(), buf.len(), fmt, (int)ctx->left_pad, (long long)val);
	}
	else {
		len = stbsp_snprintf((char*)buf.data(), buf.len(), "!< INVALID BASE: %d >", ctx->base);
	}
	return String::from_bytes(buf[{0, len}]);
}

template<typename T>
concept Formattable = requires(FormattingContext* ctx, T const& obj){
	{ format(ctx, obj) } -> ConvertibleTo<String>;
};

int main(){
	auto buf = heap_allocator()->make<byte>(512);
	auto s = format_bool(buf, true);
	std::cout << s << "...\n";
	static_assert(Formattable<i64>);

	// using namespace kielo;
	// String src =
	// 	"fn main(){\n"
	// 	"  let x: f32 = (6 + 4) / 2;\n"
	// 	"}\n"
	// ;

	// auto lex = Lexer::create(src);

	// for(auto token = lex.next().or_else({});
	// 	token.type != TokenType::EndOfFile;
	// 	token = lex.next().or_else({}))
	// {
	// 	std::cout << token << std::endl;
	// }
}

