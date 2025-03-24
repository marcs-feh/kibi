#include "core/core.hpp"

#include <iostream>

using namespace core;

enum class TokenType : i32 {
	Unknown = 0,

	ParenOpen,
	ParenClose,
	SquareOpen,
	SquareClose,
	CurlyOpen,
	CurlyClose,

	Identifier,
	String,
	Real,
	Integer,

	Plus,
	Minus,
	Slash,
	Star,
	Mod,
	
	PlusAssign,
	MinusAssign,
	SlashAssign,
	StarAssign,
	ModAssign,

	ArrowRight,

	True, False,

	EndOfFile,
};


constexpr String token_type_name(TokenType t){
	using T = TokenType;

	switch(t){
	case T::Unknown: return "<Unknown>";

	case T::ParenOpen: return "(";
	case T::ParenClose: return ")";
	case T::SquareOpen: return "[";
	case T::SquareClose: return "]";
	case T::CurlyOpen: return "{";
	case T::CurlyClose: return "}";

	case T::Identifier: return "Id";
	case T::String: return "Str";
	case T::Real: return "Real";
	case T::Integer: return "Int";

	case T::True: return "true";
	case T::False: return "false";

	case T::Plus: return "+"; break;
	case T::Minus: return "-"; break;
	case T::Slash: return "/"; break;
	case T::Star: return "*"; break;
	case T::Mod: return "%"; break;
	
	case T::PlusAssign: return "+="; break;
	case T::MinusAssign: return "-="; break;
	case T::SlashAssign: return "/="; break;
	case T::StarAssign: return "*="; break;
	case T::ModAssign: return "%="; break;

	case T::ArrowRight: return "->"; break;

	case T::EndOfFile: return "<EOF>";
	}

	panic("Unknown token type");
};

struct Token {
	String lexeme;
	TokenType type;
	i64 offset;

	Token() : lexeme{""}, type{0}, offset{0}{}
};

struct Lexer {
	i64 current;
	i64 previous;
	Slice<byte> source;

	rune advance();

	bool advance_matching(rune desired);

	void rewind();

	rune peek();

	Token make_token(TokenType t);

	Token next();

	Token consume_line_comment(){ panic("unimplemented"); }

	static Lexer create(String source);

	Lexer() : current{0}, previous{0}, source{} {}
};

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
	}

	return token;
}

#undef MATCH_NEXT
#undef MATCH_DEFAULT

std::ostream& operator<<(std::ostream& os, String s){
	os.write(s.data(), s.len());
	return os;
}


int main(){
	String src =
		"%%="
	;

	auto lex = Lexer::create(src);

	for(auto tok = lex.next(); tok.type != TokenType::EndOfFile; tok = lex.next()){
		std::cout << token_type_name(tok.type) << std::endl;
	}
}

