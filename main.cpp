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
	String source;

};

int main(){
}
