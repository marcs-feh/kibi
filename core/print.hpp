#pragma once
#include "core.hpp"
#include "memory.hpp"

namespace core {
constexpr isize print_buffer_size = 2 * 1024 * 1024;

template<typename T>
concept StringConvertible = requires(T v, Slice<byte> buf){
	{ into_string(v, buf) } -> SameAs<Maybe<String>>;
};

Slice<byte> print_get_temporary_buffer();

template<StringConvertible T, StringConvertible... Args>
void print(T const& v, Args const& ... args){
	auto buf = print_get_temporary_buffer();
	buf_print(buf, v, args...);
}

template<StringConvertible T>
void buf_print(Slice<byte> buf, T const& v){
	auto s = into_string(v, buf).or_else("!< PRINT ERROR >");
	printf("%.*s\n", (int)s.len(), (char*)s.data());
}

template<StringConvertible T, StringConvertible... Args>
void buf_print(Slice<byte> buf, T const& v, Args const& ... args){
	auto s = into_string(v, buf).or_else("!< PRINT ERROR >");
	printf("%.*s ", (int)s.len(), (char*)s.data());
	buf_print(buf, args...);
}

Maybe<String> into_string(String v, Slice<byte>);

Maybe<String> into_string(i64 v, Slice<byte> buf);

Maybe<String> into_string(uintptr v, Slice<byte> buf);

Maybe<String> into_string(f64 v, Slice<byte> buf);

template<StringConvertible T>
Maybe<String> into_string(Slice<T> s, Slice<byte> buf){
	if(s.len() == 0){ return String("[]"); }
	if(buf.len() < 4){ return {}; }

	isize current = 1;
	buf[0] = '[';
	auto remaining = buf[{current, buf.len()}];

	for(auto& e : s){
		auto res = into_string(e, remaining);
		if(res.ok()){
			current += res.unwrap().len();
		} else {
			return {};
		}

		if((current + 1) > buf.len()){ return {}; }
		current += 1;
		buf[current - 1] = ' ';

		remaining = buf[{current, buf.len()}];
	}

	if(current >= buf.len()){ return {}; }
	buf[current - 1] = ']';

	return String::from_bytes(buf[{0, current}]);
}

}
