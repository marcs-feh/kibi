#define STB_SPRINTF_IMPLEMENTATION

#include "print.hpp"
#include "stb_sprintf.h"

namespace core {

Slice<byte> print_get_temporary_buffer(){
	thread_local byte buf[print_buffer_size];
	return Slice<byte>(&buf[0], print_buffer_size);
}

Maybe<String> into_string(i64 v, Slice<byte> buf){
	isize len = stbsp_snprintf((char*)buf.data(), (int)buf.len(), "%lld", (long long)v);
	if(len >= buf.len()){
		return {};
	}
	return String::from_bytes(buf[{0, len}]);
}

Maybe<String> into_string(uintptr v, Slice<byte> buf){
	isize len = stbsp_snprintf((char*)buf.data(), (int)buf.len(), "%p", (void*)v);
	if(len >= buf.len()){
		return {};
	}
	return String::from_bytes(buf[{0, len}]);
}

Maybe<String> into_string(f64 v, Slice<byte> buf){
	isize len = stbsp_snprintf((char*)buf.data(), (int)buf.len(), "%f", v);
	if(len >= buf.len()){
		return {};
	}
	return String::from_bytes(buf[{0, len}]);
}

Maybe<String> into_string(String v, Slice<byte>){
	return v;
}

}
