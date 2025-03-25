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

#include <math.h>

int main(){
	using namespace kielo;
	String src =
		"//+-*/%&|~\n"
		"<<<^>>>"
	;

	auto lex = Lexer::create(src);

	for(auto token = lex.next().or_else({});
		token.type != TokenType::EndOfFile;
		token = lex.next().or_else({}))
	{
		std::cout << token_type_name(token.type) << std::endl;
	}

}

