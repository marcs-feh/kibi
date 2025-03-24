#include "core.hpp"

namespace core {

constexpr i32 UTF8_RANGE1 = 0x7f;
constexpr i32 UTF8_RANGE2 = 0x7ff;
constexpr i32 UTF8_RANGE3 = 0xffff;
constexpr i32 UTF8_RANGE4 = 0x10ffff;

constexpr i32 UTF16_SURROGATE1 = 0xd800;
constexpr i32 UTF16_SURROGATE2 = 0xdfff;

constexpr u32 UTF8_MASK2 = 0b0001'1111;
constexpr u32 UTF8_MASK3 = 0b0000'1111;
constexpr u32 UTF8_MASK4 = 0b0000'0111;

constexpr u32 UTF8_MASKX = 0b0011'1111;

constexpr u32 UTF8_SIZE2 = 0xc0; /* 110x_xxxx */
constexpr u32 UTF8_SIZE3 = 0xe0; /* 1110_xxxx */
constexpr u32 UTF8_SIZE4 = 0xf0; /* 1111_0xxx */

constexpr u32 CONT = 0x80;  /* 10xx_xxxx */

constexpr rune UTF8_ERROR = 0xfffd;

constexpr UTF8Encode UTF8_ERROR_ENCODED = {
	.bytes = {0xef, 0xbf, 0xbd},
	.len = 0,
};

u32 utf8_rune_size(rune c){
	if(0){}
	else if(c <= UTF8_RANGE1){ return 1; }
	else if(c <= UTF8_RANGE2){ return 2; }
	else if(c <= UTF8_RANGE3){ return 3; }
	else if(c <= UTF8_RANGE4){ return 4; }
	return 0;
}

UTF8Encode utf8_encode(rune c){
	UTF8Encode res = {};

	if(utf8_is_continuation_byte(c) ||
	   (c >= UTF16_SURROGATE1 && c <= UTF16_SURROGATE2) ||
	   (c > UTF8_RANGE4))
	{
		return UTF8_ERROR_ENCODED;
	}

	if(c <= UTF8_RANGE1){
		res.len = 1;
		res.bytes[0] = u8(c & 0xff);
	}
	else if(c <= UTF8_RANGE2){
		res.len = 2;
		res.bytes[0] = UTF8_SIZE2 | ((c >> 6) & UTF8_MASK2);
		res.bytes[1] = CONT       | ((c >> 0) & UTF8_MASKX);
	}
	else if(c <= UTF8_RANGE3){
		res.len = 3;
		res.bytes[0] = UTF8_SIZE3 | ((c >> 12) & UTF8_MASK3);
		res.bytes[1] = CONT  | ((c >> 6) & UTF8_MASKX);
		res.bytes[2] = CONT  | ((c >> 0) & UTF8_MASKX);
	}
	else if(c <= UTF8_RANGE4){
		res.len = 4;
		res.bytes[0] = UTF8_SIZE4 | ((c >> 18) & UTF8_MASK4);
		res.bytes[1] = CONT  | ((c >> 12) & UTF8_MASKX);
		res.bytes[2] = CONT  | ((c >> 6)  & UTF8_MASKX);
		res.bytes[3] = CONT  | ((c >> 0)  & UTF8_MASKX);
	}
	return res;
}

static const UTF8Decode DECODE_ERROR = { .codepoint = UTF8_ERROR, .len = 1 };

UTF8Decode utf8_decode(Slice<byte> s){
	UTF8Decode res = {};
	byte const* buf = s.data();
	isize len = s.len();

	if(buf == nullptr || len <= 0){ return DECODE_ERROR; }

	byte first = buf[0];

	if((first & CONT) == 0){
		res.len = 1;
		res.codepoint |= first;
	}
	else if ((first & ~UTF8_MASK2) == UTF8_SIZE2 && len >= 2){
		res.len = 2;
		res.codepoint |= (buf[0] & UTF8_MASK2) << 6;
		res.codepoint |= (buf[1] & UTF8_MASKX) << 0;
	}
	else if ((first & ~UTF8_MASK3) == UTF8_SIZE3 && len >= 3){
		res.len = 3;
		res.codepoint |= (buf[0] & UTF8_MASK3) << 12;
		res.codepoint |= (buf[1] & UTF8_MASKX) << 6;
		res.codepoint |= (buf[2] & UTF8_MASKX) << 0;
	}
	else if ((first & ~UTF8_MASK4) == UTF8_SIZE4 && len >= 4){
		res.len = 4;
		res.codepoint |= (buf[0] & UTF8_MASK4) << 18;
		res.codepoint |= (buf[1] & UTF8_MASKX) << 12;
		res.codepoint |= (buf[2] & UTF8_MASKX) << 6;
		res.codepoint |= (buf[3] & UTF8_MASKX) << 0;
	}
	else {
		return DECODE_ERROR;
	}

	// Validation step
	if(res.codepoint >= UTF16_SURROGATE1 && res.codepoint <= UTF16_SURROGATE2){
		return DECODE_ERROR;
	}
	if(res.len > 1 && !utf8_is_continuation_byte(buf[1])){
		return DECODE_ERROR;
	}
	if(res.len > 2 && !utf8_is_continuation_byte(buf[2])){
		return DECODE_ERROR;
	}
	if(res.len > 3 && !utf8_is_continuation_byte(buf[3])){
		return DECODE_ERROR;
	}

	return res;
}

// // Steps iterator forward and puts rune and Length advanced into pointers,
// // returns false when finished.
// bool utf8_iter_next(UTF8Iterator* iter, UTF8Decode* out){
// 	if(iter->current >= iter->len){ return 0; }
//
// 	*out = utf8_decode(&iter->data[iter->current], iter->len - iter->current);
//
// 	iter->current += out->len;
//
// 	return 1;
// }
//
// // Steps iterator backward and puts rune and its length into pointers,
// // returns false when finished.
// bool utf8_iter_prev(UTF8Iterator* iter, UTF8Decode* out){
// 	if(iter->current <= 0){ return false; }
//
// 	iter->current -= 1;
// 	while(utf8_is_continuation_byte(iter->data[iter->current])){
// 		iter->current -= 1;
// 	}
//
// 	*out = utf8_decode(&iter->data[iter->current], iter->len - iter->current);
// 	return true;
// }
//
// #undef CONT
//
// bool str_empty(String s){
// 	return s.len == 0 || s.v == nullptr;
// }
//
// String str_from(char const * data){
// 	String s = {
// 		.v = (byte const *)data,
// 		.len = cstring_len(data),
// 	};
// 	return s;
// }
//
// String str_from_bytes(byte const* data, isize length){
// 	String s = {
// 		.v = (byte const *)data,
// 		.len = length,
// 	};
// 	return s;
// }
//
// String str_from_range(char const * data, isize start, isize length){
// 	String s = {
// 		.v = (byte const *)&data[start],
// 		.len = length,
// 	};
// 	return s;
// }
//
// isize str_codepoint_count(String s){
// 	UTF8Iterator it = str_iterator(s);
//
// 	isize count = 0;
// 	UTF8Decode dec = {0};
// 	while(utf8_iter_next(&it, &dec)){
// 		count += 1;
// 	}
// 	return count;
// }
//
// bool str_starts_with(String s, String prefix){
// 	if(prefix.len > s.len){ return false; }
//
// 	s = str_sub(s, 0, prefix.len);
//
// 	i32 res = mem_compare(prefix.v, s.v, prefix.len);
// 	return res == 0;
// }
//
// bool str_ends_with(String s, String suffix){
// 	if(suffix.len > s.len){ return false; }
//
// 	s = str_sub(s, s.len - suffix.len, suffix.len);
//
// 	i32 res = mem_compare(suffix.v, s.v, suffix.len);
// 	return res == 0;
// }
//
// isize str_codepoint_offset(String s, isize n){
// 	UTF8Iterator it = str_iterator(s);
//
// 	isize acc = 0;
//
// 	UTF8Decode dec = {0};
// 	do {
// 		if(acc == n){ break; }
// 		acc += 1;
// 	} while(utf8_iter_next(&it, &dec));
//
// 	return it.current;
// }
//
// // TODO: Handle length in codepoint count
// String str_sub(String s, isize start, isize byte_count){
// 	if(start < 0 || byte_count < 0 || (start + byte_count) > s.len){ return (String){0}; }
//
// 	String sub = {
// 		.v = &s.v[start],
// 		.len = byte_count,
// 	};
//
// 	return sub;
// }
//
// bool str_eq(String a, String b){
// 	if(a.len != b.len){ return false; }
//
// 	for(isize i = 0; i < a.len; i += 1){
// 		if(a.v[i] != b.v[i]){ return false; }
// 	}
//
// 	return true;
// }
//
// // UTF8Iterator str_iterator(String s){
// // 	return (UTF8Iterator){
// // 		.current = 0,
// // 		.len     = s.len,
// // 		.data    = s.v,
// // 	};
// // }
// //
// // UTF8Iterator str_iterator_reversed(String s){
// // 	return (UTF8Iterator){
// // 		.current = s.len,
// // 		.len     = s.len,
// // 		.data    = s.v,
// // 	};
// // }
//
// #undef UTF8_RANGE1
// #undef UTF8_RANGE2
// #undef UTF8_RANGE3
// #undef UTF8_RANGE4
// #undef UTF16_SURROGATE1
// #undef UTF16_SURROGATE2
// #undef UTF8_MASK2
// #undef UTF8_MASK3
// #undef UTF8_MASK4
// #undef UTF8_MASKX
// #undef UTF8_SIZE2
// #undef UTF8_SIZE3
// #undef UTF8_SIZE4
// #undef CONT
//
// }

} /* Universal namespace */
