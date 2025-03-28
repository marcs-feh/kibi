#include "core/core.hpp"
#include "core/memory.hpp"
#include "core/print.hpp"
#include "core/stream.hpp"

#include "lexer.hpp"

using namespace core;

Arena* thread_arena(){
	constexpr isize thread_arena_size = 1 * 1024 * 1024;
	thread_local byte thread_arena_memory[thread_arena_size];
	thread_local Arena arena = Arena::create(Slice(&thread_arena_memory[0], thread_arena_size));
	return &arena;
}

int main(){
	auto nums = thread_arena()->make<f64>(30);
	for(isize i = 0; i < nums.len(); i++){
		nums[i] = i;
	}
	print(nums);

	// auto buf = thread_arena()->make<byte>(20);

	// auto tmp = thread_arena()->make<byte>(100);

	// auto bs = ByteBufferStream::from(buf);
	// print(buf);

	// bs.write(String("Hello").raw_bytes());
	// print(buf);

	// bs.seek(0, SeekPos::Start);
	// isize n = bs.read(tmp).unwrap();
	// print(tmp[{0, n}]);

	// n = bs.read(tmp).or_else(0);
	// print(tmp[{0, n}]);

	// print("Nums:", nums);
}

