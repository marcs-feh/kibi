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
String format(Slice<byte> buf, i64 val, i32 base = 10, i32 left_pad = 0, bool pad_with_zero = false){
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
		switch (base) {
			case 16: fmt = hex_fmt_z; break;
			case 10: fmt = dec_fmt_z; break;
			case 8:  fmt = oct_fmt_z; break;
			case 2:  fmt = bin_fmt_z; break;
		}
	}
	else {
		switch (base) {
			case 16: fmt = hex_fmt; break;
			case 10: fmt = dec_fmt; break;
			case 8:  fmt = oct_fmt; break;
			case 2:  fmt = bin_fmt; break;
		}
	}

	isize len = 0;
	if(fmt != nullptr){
		len = stbsp_snprintf((char*)buf.data(), buf.len(), fmt, (int)left_pad, (long long)val);
	}
	else {
		len = stbsp_snprintf((char*)buf.data(), buf.len(), "!< INVALID BASE: %d >", base);
	}
	return String::from_bytes(buf[{0, len}]);
}

int main(){
	auto buf = heap_allocator()->make<byte>(512);
	auto s = format(buf, 6900000000, 10);
	std::cout << s << '\n';

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

