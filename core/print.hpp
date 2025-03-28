#pragma once
#include "core.hpp"
#include "memory.hpp"
#include "stream.hpp"

namespace core {
constexpr isize print_buffer_size = 2 * 1024 * 1024;

template<typename T>
concept StringConvertible = requires(T v, ByteBufferStream& buf){
{ into_string(v, buf) } -> SameAs<Maybe<String>>;
};

Slice<byte> print_get_temporary_buffer();

isize format_write_i64(Slice<byte> buf, i64 v);

isize format_write_u64(Slice<byte> buf, u64 v);

isize format_write_f64(Slice<byte> buf, f64 v);

template<StringConvertible T, StringConvertible... Args>
void print(T const& v, Args const& ... args){
	auto buf = print_get_temporary_buffer();
	buf_print(buf, v, args...);
}

template<StringConvertible T>
void buf_print(Slice<byte> buf, T const& v){
	auto stream = ByteBufferStream::from(buf);
	auto s = into_string(v, stream).or_else("!< PRINT ERROR >");
	printf("%.*s\n", (int)s.len(), (char*)s.data());
}

template<StringConvertible T, StringConvertible... Args>
void buf_print(Slice<byte> buf, T const& v, Args const& ... args){
	auto stream = ByteBufferStream::from(buf);
	auto s = into_string(v, stream).or_else("!< PRINT ERROR >");
	printf("%.*s ", (int)s.len(), (char*)s.data());
	buf_print(buf, args...);
}

Maybe<String> into_string(String v, ByteBufferStream& buf);

template<typename Int>
	requires(Signed<Int> && Integral<Int>)
Maybe<String> into_string(Int v, ByteBufferStream& buf){
	isize start = buf.current;

	auto target = buf[{buf.current, buf.len()}];
	isize len = format_write_i64(target, v);
	if(len >= buf.len()){
		return {};
	}
	buf.current += len;
	return String::from_bytes(buf[{start, start + len}]);
}

template<typename Int>
	requires(!Signed<Int> && Integral<Int>)
Maybe<String> into_string(Int v, ByteBufferStream& buf){
	isize start = buf.current;

	auto target = buf[{buf.current, buf.len()}];
	isize len = format_write_u64(target, v);
	if(len >= buf.len()){
		return {};
	}
	buf.current += len;
	return String::from_bytes(buf[{start, start + len}]);
}

Maybe<String> into_string(FloatingPoint auto v, ByteBufferStream& buf){
	isize start = buf.current;
	auto target = buf[{buf.current, buf.len()}];
	isize len = format_write_f64(target, v);
	if(len >= buf.len()){
		return {};
	}
	buf.current += len;
	return String::from_bytes(buf[{start, start + len}]);
}

Maybe<String> into_string(void const* v, ByteBufferStream& buf);

template<Integral T>
struct HexInteger { T value; };

template<StringConvertible T>
Maybe<String> into_string(Slice<T> s, ByteBufferStream& buf){
	if(buf.len() < 4){ return {}; }
	if(s.len() == 0){
		return String("[]");
	}
	isize restore_point = buf.current;

	if(!buf.write(String("[").raw_bytes()).ok()){
		goto error_exit;
	}

	for(auto& e : s){
		auto res = into_string(e, buf);
		if(!res.ok()){
			goto error_exit;
		}

		if(!buf.write(String(" ").raw_bytes()).ok()){
			goto error_exit;
		}
	}

	buf.seek(-1, SeekPos::Current);
	if(!buf.write(String("]").raw_bytes()).ok()){
		goto error_exit;
	}

	return String::from_bytes(Slice<byte>(buf.data() + restore_point, restore_point - buf.current));

error_exit:
	buf.seek(restore_point, SeekPos::Start);
	return {};
}
} /* namespace core */
