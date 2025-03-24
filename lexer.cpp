#include "core/core.hpp"
#include "lexer.hpp"

using namespace core;


Slice<Pair<String, TokenType>> lexer_operator_table(){
	static bool sorted = false;
	using T = TokenType;
	static Pair<String, TokenType> _lexer_operators[] = {
		// {"(", ParenOpen},
		// {")", ParenClose},
		// {"[", SquareOpen},
		// {"]", SquareClose},
		// {"{", CurlyOpen},
		// {"}", CurlyClose},
		// {"+", Plus},
		// {"-", Minus},
		// {"*", Star},
		// {"/", Slash},
		// {"%", Mod},
		// {"&", And},
		// {"|", Or},
		// {"~", Tilde},
		// {">>", ShiftRight},
		// {"<<", ShiftLeft},
		// {"&&", LogicAnd},
		// {"||", LogicOr},
		// {"!", LogicNot},
		// {">", Greater},
		// {"<", Less},
		// {">=", GreaterEqual},
		// {"<=", LessEqual},
		// {"==", Equal},
		// {"!=", NotEqual},
		// {"=", Assign},
		// {"+=", PlusAssign},
		// {"-=", MinusAssign},
		// {"*=", StarAssign},
		// {"/=", SlashAssign},
		// {"%=", ModAssign},
		// {"&=", AndAssign},
		// {"|=", OrAssign},
		// {"->", ArrowRight},
	};

	if(!sorted){
		// sort
	}
}
