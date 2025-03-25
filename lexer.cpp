#include "core/core.hpp"
#include "core/memory.hpp"
#include "lexer.hpp"

namespace kielo {
Lexer Lexer::create(String source){
	Lexer lex;
	lex.source = source.raw_bytes();
	return lex;
}

static inline
bool is_decimal_digit(rune c){
	return (c >= '0') && (c <= '9');
}

static inline
bool is_identifier_char(rune c){
	auto upper_ascii = (c >= 'A') && (c <= 'Z');
	auto lower_ascii = (c >= 'a') && (c <= 'z');
	auto decimal = (c >= '0') && (c <= '9');
	return upper_ascii || lower_ascii || decimal || (c == '_');
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

static inline
Maybe<TokenType> keyword_of(String s){
	using T = TokenType;
	constexpr Pair<String, TokenType> keywords[] = {
		{"if",       T::If},
		{"else",     T::Else},
		{"for",      T::For},
		{"break",    T::Break},
		{"continue", T::Continue},
		{"fn",       T::Fn},
		{"return",   T::Return},
		{"match",    T::Match},
		{"let",      T::Let},
		{"struct",   T::Struct},
	};

	constexpr isize N = sizeof(keywords) / sizeof(keywords[0]);

	for(isize i = 0; i < N; i += 1){
		if(s == keywords[i].a){
			return keywords[i].b;
		}
	}
	return {};
}


Token Lexer::consume_identifier(){
	previous = current;

	for(;;){
		rune c = advance();
		if(!is_identifier_char(c)){
			rewind();
			break;
		}
	}

	auto token = make_token(TokenType::Unknown);
	token.type = keyword_of(token.lexeme).or_else(TokenType::Identifier);
	return token;
}

Token Lexer::consume_line_comment(){
    previous = current;

    for(;;){
        rune c = advance();
        if(c == '\n' || c == 0){
            break;
        }
    }
    
    auto res = make_token(TokenType::LineComment);

	// Include double-slash and ignore line-feed
	res.lexeme = String::from_bytes(source[{previous-2, current-1}]);
	return res;
}

Error Lexer::make_error(ErrorType t){
	Error e;
	e.type = t;
	e.offset = current;
	// e.file = file;
	return e;
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

Result<Token, Error> Lexer::next(){
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

		case '.':
			MATCH_DEFAULT(make_token(T::Dot));

		case '^':
			MATCH_DEFAULT(make_token(T::Caret));

		case ':':
			MATCH_DEFAULT(make_token(T::Colon));

		case ',':
			MATCH_DEFAULT(make_token(T::Comma));

		case ';':
			MATCH_DEFAULT(make_token(T::Semicolon));

		case '{':
			MATCH_DEFAULT(make_token(T::CurlyOpen));

		case '}':
			MATCH_DEFAULT(make_token(T::CurlyClose));

		case '+':
			MATCH_NEXT('=', make_token(T::PlusAssign));
			MATCH_DEFAULT(make_token(T::Plus));

		case '=':
			MATCH_NEXT('=', make_token(T::Equal));
			MATCH_DEFAULT(make_token(T::Assign));

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

		case '"':
			panic("Unimplemented: string");

		case '\n': case '\r': case '\t': case ' ':
			token = make_token(T::Whitespace);

		default:
			if(is_decimal_digit(c)){
				panic("Unimplemented: number");
			}
			else if(is_identifier_char(c)){
				rewind();
				token = consume_identifier();
			}
		break;
	}

	if(token.type == TokenType::Unknown)
		return make_error(ErrorType::Lexer_BadCodepoint);
	else
		return token;
}

#undef MATCH_NEXT
#undef MATCH_DEFAULT

} /* Namespace */
