#include "core/core.hpp"
#include "core/memory.hpp"


#include <iostream>
#include "lexer.cpp"

using namespace core;

Lexer Lexer::create(String source){
	Lexer lex;
	lex.source = source.raw_bytes();
	return lex;
}

rune Lexer::peek(){
	if(current >= source.len()){ return 0; }
	auto [codepoint, _] = utf8_decode(source[{current, source.len()}]);
	return codepoint;
}

bool Lexer::advance_matching(rune desired){
	rune c = peek();
	if(c == desired){
		current += utf8_rune_size(c);
		return true;
	}

	return false;
}

rune Lexer::advance(){
	if(current >= source.len()){ return 0; }

	auto [codepoint, n] = utf8_decode(source[{current, source.len()}]);
	current += n;
	return codepoint;
}

void Lexer::rewind(){
	if(current <= 0){ return; }
	current -= 1;
	while((current != 0) && utf8_is_continuation_byte(source[current])){
		current -= 1;
	}
}

Token Lexer::make_token(TokenType t){
	Token token;
	token.type = t;
	token.lexeme = String::from_bytes(source[{previous, current}]);
	token.offset = current;
	return token;
}

#define MATCH_NEXT(Char, Expr) if(advance_matching(Char)){ token = (Expr); break; }
#define MATCH_DEFAULT(Expr) { token = (Expr); break; }

Token Lexer::next(){
	Token token;
	previous = current;

	rune c = advance();
	if(c == 0){
		return make_token(TokenType::EndOfFile);
	}

	using T = TokenType;
	switch(c){
		case '(':
			MATCH_DEFAULT(make_token(T::ParenOpen));

		case ')':
			MATCH_DEFAULT(make_token(T::ParenClose));

		case '[':
			MATCH_DEFAULT(make_token(T::SquareOpen));

		case ']':
			MATCH_DEFAULT(make_token(T::SquareClose));

		case '{':
			MATCH_DEFAULT(make_token(T::CurlyOpen));

		case '}':
			MATCH_DEFAULT(make_token(T::CurlyClose));

		case '+':
			MATCH_NEXT('=', make_token(T::PlusAssign));
			MATCH_DEFAULT(make_token(T::Plus));

		// case '.':
		// 	MATCH_DEFAULT(make_token(T::Dot));

		case '-':
			MATCH_NEXT('=', make_token(T::MinusAssign));
			MATCH_NEXT('>', make_token(T::ArrowRight));
			MATCH_DEFAULT(make_token(T::Minus));

		case '*':
			MATCH_NEXT('=', make_token(T::StarAssign));
			MATCH_DEFAULT(make_token(T::Star));

		case '/':
			MATCH_NEXT('/', consume_line_comment());
			MATCH_NEXT('=', make_token(T::SlashAssign));
			MATCH_DEFAULT(make_token(T::Slash));

		case '%':
			MATCH_NEXT('=', make_token(T::ModAssign));
			MATCH_DEFAULT(make_token(T::Mod));

		case '>':
			MATCH_NEXT('>', make_token(T::ShiftRight));
			MATCH_NEXT('=', make_token(T::GreaterEqual));
			MATCH_DEFAULT(make_token(T::Greater));

		case '<':
			MATCH_NEXT('<', make_token(T::ShiftLeft));
			MATCH_NEXT('=', make_token(T::LessEqual));
			MATCH_DEFAULT(make_token(T::Less));

		case '&':
			MATCH_NEXT('=', make_token(T::AndAssign));
			MATCH_NEXT('&', make_token(T::LogicAnd));
			MATCH_DEFAULT(make_token(T::And));

		case '|':
			MATCH_NEXT('=', make_token(T::OrAssign));
			MATCH_NEXT('|', make_token(T::LogicOr));
			MATCH_DEFAULT(make_token(T::Or));

		case '~':
			MATCH_DEFAULT(make_token(T::Tilde));

		case '!':
			MATCH_NEXT('=', make_token(T::NotEqual));
			MATCH_DEFAULT(make_token(T::LogicNot));
	}

	return token;
}

#undef MATCH_NEXT
#undef MATCH_DEFAULT

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
	String src =
		"+-*/%&|~"
	;


	auto ops = lexer_operator_table();
	for(auto [name, _] : ops){
		std::cout << name << '\n';
	}

	// auto lex = Lexer::create(src);
	//
	// for(auto tok = lex.next(); tok.type != TokenType::EndOfFile; tok = lex.next()){
	// 	std::cout << token_type_name(tok.type) << std::endl;
	// }

}

