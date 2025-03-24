#include "core/core.hpp"

using namespace core;

enum class TokenType : u8 {
	Unknown = 0,

	ParenOpen,
	ParenClose,
	SquareOpen,
	SquareClose,
	CurlyOpen,
	CurlyClose,
};

struct Lexer {
	i64 current;
	i64 previous;
	Slice<byte> source;

	rune advance();

	bool advance_matching(rune desired);

	void rewind();

	rune peek();
};

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
	while(current != 0 && utf8_is_continuation_byte(source[current])){
		current -= 1;
	}
}

int main(){
}
