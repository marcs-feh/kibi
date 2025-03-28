#pragma once

#include "core.hpp"

namespace core {

enum class SeekPos : u8 {
	Start = 0,
	Current = 1,
	End = 2,
};

enum class StreamError : u32 {
    None = 0,

    BrokenChannel,
    Closed,
    Unsupported,
	InvalidOffset,
	InvalidWhence,
	RequestTooBig,
    EndOfStream,
};

using StreamMode = u8;
constexpr inline StreamMode stream_mode_read  = (1 << 0);
constexpr inline StreamMode stream_mode_write = (1 << 1);
constexpr inline StreamMode stream_mode_seek  = (1 << 2);

struct Stream {
    virtual Result<i64, StreamError> read(Slice<byte> buf, i64 offset = 0, SeekPos whence = SeekPos::Current) = 0;
    virtual Result<i64, StreamError> write(Slice<byte> buf, i64 offset = 0, SeekPos whence = SeekPos::Current) = 0;
    virtual Result<i64, StreamError> seek(i64 offset, SeekPos whence) = 0;
    virtual StreamMode capabilities() = 0;
};

// Simple stream to operate over byte buffers, useful for building strings and
// binary payloads with a maximum size.
struct ByteBufferStream : Stream {
	Slice<byte> buffer;
	isize current;

    Result<i64, StreamError> read(Slice<byte> buf, i64 offset = 0, SeekPos whence = SeekPos::Current) override;

    Result<i64, StreamError> write(Slice<byte> buf, i64 offset = 0, SeekPos whence = SeekPos::Current) override;

    Result<i64, StreamError> seek(i64 offset, SeekPos whence) override;

    StreamMode capabilities() override;

	ByteBufferStream()
		: buffer{}
		, current{0} {}

	static ByteBufferStream from(Slice<byte> buf){
		ByteBufferStream bs;
		bs.buffer = buf;
		return bs;
	}

	Slice<byte> operator[](Pair<isize> range){
		return buffer[range];
	}

	// Getters
	[[nodiscard]] constexpr forceinline auto len() const { return buffer.len(); }
	[[nodiscard]] constexpr forceinline auto data() const { return buffer.data(); }
};

}
