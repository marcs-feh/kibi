#include "core/core.hpp"
#include "core/memory.hpp"
#include "core/print.hpp"
#include "core/stream.hpp"

#include "lexer.hpp"

using namespace core;

struct ByteBufferStream : Stream {
	Slice<byte> data;
	isize current;

    Result<i64, StreamError> read(Slice<byte> buf, i64 offset = 0, SeekPos whence = SeekPos::Current) override {
		const auto start = data.data();
		const auto end = data.data() + data.len();
		const byte* pos = nullptr;

		switch (whence) {
		case SeekPos::Start:   pos = start + offset; break;
		case SeekPos::Current: pos = start + offset + current; break;
		case SeekPos::End:     pos = end + offset; break;
		default:
			return StreamError::InvalidWhence;
		}

		if(pos < start || pos >= end){
			return StreamError::InvalidOffset;
		}
		isize remaining = end - pos;
		auto to_read = min(remaining, buf.len());

		mem_copy(buf.data(), pos, to_read);
		current += to_read;
		return to_read;
	}

    Result<i64, StreamError> write(Slice<byte> buf, i64 offset = 0, SeekPos whence = SeekPos::Current) override {
		const auto start = data.data();
		const auto end = data.data() + data.len();
		byte* pos = nullptr;

		switch (whence) {
		case SeekPos::Start:   pos = start; break;
		case SeekPos::Current: pos = start + current; break;
		case SeekPos::End:     pos = end ; break;
		default:
			return StreamError::InvalidWhence;
		}
		pos += offset;

		if(pos < start || pos >= end){
			return StreamError::InvalidOffset;
		}
		isize remaining = end - pos;
		auto to_write = min(remaining, buf.len());
		mem_copy(pos, buf.data(), to_write);
		current += to_write;
		return to_write;
	}

    Result<i64, StreamError> seek(i64 offset, SeekPos whence) override {
		isize pos = 0;

		switch(whence){
		case SeekPos::Start:   pos = 0; break;
		case SeekPos::Current: pos = current; break;
		case SeekPos::End:     pos = data.len(); break;
		default: return StreamError::InvalidWhence;
		}
		pos += offset;

		if(pos < 0 || pos >= data.len()){
			return StreamError::InvalidOffset;
		}

		current = pos;
		return current;
	}

    StreamMode capabilities() override {
		return stream_mode_read | stream_mode_write | stream_mode_seek;
	}

	ByteBufferStream()
		: data{}
		, current{0} {}

	static ByteBufferStream create(Slice<byte> buf){
		ByteBufferStream bs;
		bs.data = buf;
		return bs;
	}
};

Arena* thread_arena(){
	constexpr isize thread_arena_size = 1 * 1024 * 1024;
	thread_local byte thread_arena_memory[thread_arena_size];
	thread_local Arena arena = Arena::create(Slice(&thread_arena_memory[0], thread_arena_size));
	return &arena;
}

int main(){
	auto nums = thread_arena()->make<f64>(30);
	for(isize i = 0; i < nums.len(); i++)
		nums[i] = i / 2.0;

	auto buf = thread_arena()->make<byte>(20);

	auto tmp = thread_arena()->make<byte>(5);

	auto bs = ByteBufferStream::create(buf);
	print(buf);

	bs.write(String("Hello").raw_bytes());
	print(buf);

	bs.seek(-2, SeekPos::Start);
	isize n = bs.read(tmp).or_else(0);
	print(tmp[{0, n}]);

	print("Nums:", nums);
}

