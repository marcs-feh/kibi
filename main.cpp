#include "core/core.hpp"
#include "core/memory.hpp"
#include "core/format.hpp"

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

namespace foo {
struct Person {
	char const* name;
};

core::String format(core::FormattingContext* ctx, Person const& p){
	return "<>";
}

}

#define STB_SPRINTF_IMPLEMENTATION
#include "core/wip/stb_sprintf.h"

using namespace core;


int main(){
	auto buf = heap_allocator()->make<byte>(512);
	// auto s = format(buf, true);
	// std::cout << s << "...\n";
	static_assert(Formattable<foo::Person>);
	

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

