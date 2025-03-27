#pragma once

#include "core.hpp"

namespace core {

enum class StreamError : u32 {
    None = 0,

    BrokenChannel,
    Closed,
    Unsupported,
	RequestTooBig,
    EndOfStream,
};

using StreamMode = u8;
constexpr StreamMode stream_mode_read = (1 << 0);
constexpr StreamMode stream_mode_write = (1 << 1);

struct Stream {
    virtual Result<u64, StreamError> read(Slice<byte> buf) = 0;
    virtual Result<u64, StreamError> write(Slice<byte> buf) = 0;
    virtual StreamMode capabilities() = 0;
};

struct ByteStream : Stream {
	Slice<byte> buffer;
	isize last_read;
	isize last_write;

    Result<u64, StreamError> read(Slice<byte> out) override {
	}

    Result<u64, StreamError> write(Slice<byte> s) override {
	}

	void reset(){
		last_read = 0;
	}

    StreamMode capabilities() override {
		return stream_mode_read | stream_mode_write;
	}

	static ByteStream create(Slice<byte> buf){
		ByteStream s;
		s.buffer = buf;
		return s;
	}

	ByteStream(): buffer{}, last_read{0}, last_write{0} {}
};

}
