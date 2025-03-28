#include "stream.hpp"

namespace core {
Result<i64, StreamError> ByteBufferStream::read(Slice<byte> buf, i64 offset, SeekPos whence) {
	const auto start = buffer.data();
	const auto end   = buffer.data() + buffer.len();
	const byte* pos  = nullptr;

	switch (whence) {
	case SeekPos::Start:   pos = start; break;
	case SeekPos::Current: pos = start + current; break;
	case SeekPos::End:     pos = end; break;
	default: return StreamError::InvalidWhence;
	}

	pos += offset;
	if(pos < start || pos >= end){
		return StreamError::InvalidOffset;
	}

	isize remaining = end - pos;
	auto to_read = min(remaining, buf.len());
	mem_copy(buf.data(), pos, to_read);

	current += to_read;
	return to_read;
}


Result<i64, StreamError> ByteBufferStream::write(Slice<byte> buf, i64 offset, SeekPos whence) {
	const auto start = buffer.data();
	const auto end = buffer.data() + buffer.len();
	byte* pos = nullptr;

	switch (whence) {
	case SeekPos::Start:   pos = start; break;
	case SeekPos::Current: pos = start + current; break;
	case SeekPos::End:     pos = end ; break;
	default: return StreamError::InvalidWhence;
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

Result<i64, StreamError> ByteBufferStream::seek(i64 offset, SeekPos whence) {
	isize pos = 0;

	switch(whence){
	case SeekPos::Start:   pos = 0; break;
	case SeekPos::Current: pos = current; break;
	case SeekPos::End:     pos = buffer.len(); break;
	default: return StreamError::InvalidWhence;
	}

	pos += offset;
	if(pos < 0 || pos >= buffer.len()){
		return StreamError::InvalidOffset;
	}

	current = pos;
	return current;
}

StreamMode ByteBufferStream::capabilities() {
	return stream_mode_read | stream_mode_write | stream_mode_seek;
}

}
