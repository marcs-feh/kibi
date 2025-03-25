#include "core/core.hpp"
#include "core/memory.hpp"
#include "lexer.hpp"

using namespace core;


Slice<Pair<String, TokenType>> lexer_operator_table(){
	static bool sorted = false;
	using T = TokenType;

	static Pair<String, TokenType> _lexer_operators[] = {
		{"(", T::ParenOpen},
		{")", T::ParenClose},
		{"[", T::SquareOpen},
		{"]", T::SquareClose},
		{"{", T::CurlyOpen},
		{"}", T::CurlyClose},

		{"+", T::Plus},
		{"-", T::Minus},
		{"*", T::Star},
		{"/", T::Slash},
		{"%", T::Mod},
		{"&", T::And},
		{"|", T::Or},
		{"~", T::Tilde},
		{">>", T::ShiftRight},
		{"<<", T::ShiftLeft},

		{"&&", T::LogicAnd},
		{"||", T::LogicOr},
		{"!", T::LogicNot},
		{">", T::Greater},
		{"<", T::Less},
		{">=", T::GreaterEqual},
		{"<=", T::LessEqual},
		{"==", T::Equal},
		{"!=", T::NotEqual},

		{"=", T:: Assign},
		{"->", T::ArrowRight},

		{"+=", T::PlusAssign},
		{"-=", T::MinusAssign},
		{"*=", T::StarAssign},
		{"/=", T::SlashAssign},
		{"%=", T::ModAssign},
		{"&=", T::AndAssign},
		{"|=", T::OrAssign},
	};

	auto operators = Slice(&_lexer_operators[0], sizeof(_lexer_operators) / sizeof(_lexer_operators[0]));

	if(!sorted){
		sort(operators, [](auto const& lhs, auto const& rhs) -> int {
			String left = lhs.a;
			String right = rhs.a;

			int res = - mem_compare(left.data(), right.data(), min(left.len(), right.len()));
			if(res == 0) {
				res = right.len() - left.len();
			}
			return res;
		});
		sorted = true;
	}

	return operators;
}
