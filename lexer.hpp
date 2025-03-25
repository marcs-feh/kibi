#pragma once

#include "core/core.hpp"

namespace kielo {
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
	Star,
	Slash,
	Mod,

	And,
	Or,
	Tilde,
	ShiftRight,
	ShiftLeft,

	LogicAnd,
	LogicOr,
	LogicNot,

	Greater,
	Less,
	GreaterEqual,
	LessEqual,
	Equal,
	NotEqual,

	Assign,
	ArrowRight,
	Dot,
	Colon,
	Semicolon,
	Comma,

	PlusAssign,
	MinusAssign,
	StarAssign,
	SlashAssign,
	ModAssign,

	AndAssign,
	OrAssign,

	True,
	False,
	Fn,
	Let,
	Const,
	Struct,
	If,
	Else,
	Return,
	For,
	Break,
	Continue,
	Match,

	LineComment,
	EndOfFile,
};

struct Token {
	String lexeme;
	TokenType type;
	i64 offset;

	Token() : lexeme{""}, type{0}, offset{0}{}
};

enum class ErrorType : u32 {
	None = 0,

	Lexer_BadCodepoint,
};

struct Error {
	String file = "<No file>";
	String message = "";
	i64 offset = 0;
	ErrorType type{0};
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

	Error make_error(ErrorType t);

	Result<Token, Error> next();

	Token consume_line_comment();

	Result<Token, Error> consume_identifier();

	static Lexer create(String source);

	Lexer() : current{0}, previous{0}, source{} {}
};

constexpr static inline
String token_type_name(TokenType t){
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

	case T::Plus: return "+";
	case T::Minus: return "-";
	case T::Slash: return "/";
	case T::Star: return "*";
	case T::Mod: return "%";
	case T::And: return "&";
	case T::Or: return "|";
	case T::Tilde: return "~";
	case T::ShiftRight: return ">>";
	case T::ShiftLeft: return "<<";

	case T::PlusAssign: return "+=";
	case T::MinusAssign: return "-=";
	case T::SlashAssign: return "/=";
	case T::StarAssign: return "*=";
	case T::ModAssign: return "%=";
	case T::AndAssign: return "&=";
	case T::OrAssign: return "|=";

	case T::Assign: return "=";
	case T::ArrowRight: return "->";
	case T::Dot: return ".";
	case T::Colon: return ":";
	case T::Semicolon: return ";";
	case T::Comma: return ",";

	case T::LogicNot: return "!";
	case T::LogicAnd: return "&&";
	case T::LogicOr: return "||";
	case T::Greater: return ">";
	case T::Less: return "<";
	case T::GreaterEqual: return ">=";
	case T::LessEqual: return "<=";
	case T::Equal: return "==";
	case T::NotEqual: return "!=";

	case T::Fn: return "fn";
	case T::Let: return "let";
	case T::Const: return "const";
	case T::Struct: return "struct";
	case T::If: return "if";
	case T::Else: return "else";
	case T::Return: return "return";
	case T::For: return "for";
	case T::Break: return "break";
	case T::Continue: return "continue";
	case T::Match: return "match";

	case T::EndOfFile: return "<EOF>";
	}

	panic("Unknown token type");
};
}
