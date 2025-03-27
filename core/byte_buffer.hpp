#pragma once

namespace core {

struct ByteBuffer {
	DynamicArray<byte> buffer;
	isize last_write;
	isize last_read;

	i64 append(byte b){
		buffer.append(b);
		return 1;
	}

	i64 append(Slice<byte> s){
		auto old_len = buffer.len();
		buffer.append(s);
		auto inc = buffer.len() - old_len;
		return inc;
	}

	ByteBuffer()
		: buffer{},
		: last_write{0}
		, last_read{0} {}
};

}

