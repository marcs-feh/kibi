#define STB_SPRINTF_IMPLEMENTATION
#include "stb_sprintf.h"

#include "print.hpp"

namespace core {

Slice<byte> print_get_temporary_buffer(){
	thread_local byte buf[print_buffer_size];
	return Slice<byte>(&buf[0], print_buffer_size);
}

isize format_write_i64(Slice<byte> buf, i64 v){
	return stbsp_snprintf((char*)buf.data(), (int)buf.len(), "%lld", (long long)v);
}

isize format_write_u64(Slice<byte> buf, u64 v){
	return stbsp_snprintf((char*)buf.data(), (int)buf.len(), "%llu", (unsigned long long)v);
}

isize format_write_f64(Slice<byte> buf, f64 v){
	return stbsp_snprintf((char*)buf.data(), (int)buf.len(), "%g", v);
}

Maybe<String> into_string(void const* v, ByteBufferStream& buf){
	isize start = buf.current;
	isize len = stbsp_snprintf((char*)buf.data() + buf.current, (int)buf.len(), "%p", v);
	if(len >= buf.len()){
		return {};
	}
	buf.current += len;
	return String::from_bytes(buf[{start, start + len}]);
}

Maybe<String> into_string(String v, ByteBufferStream& buf){
	isize start = buf.current;
	isize len = buf.write(v.raw_bytes()).or_else(-1);
	if(len < 0){
		return {};
	}
	return String::from_bytes(buf[{start, start + len}]);
}

}
