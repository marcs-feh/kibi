#include "core/core.hpp"
#include "core/memory.hpp"
#include "core/print.hpp"

#include <iostream>
#include "lexer.hpp"

using core::String, core::Slice, core::isize;

std::ostream& operator<<(std::ostream& os, String s){
	os.write(s.data(), s.len());
	return os;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, Slice<T> s){
	for(isize i = 0; i < s.len(); i++)
		os << s[i] << ' ';
	return os;
}

std::ostream& operator<<(std::ostream& os, kielo::Token t){
	os << kielo::token_type_name(t.type) << "\t\"" << t.lexeme << '"';
	return os;
}

using namespace core;

Arena* thread_arena(){
	constexpr isize thread_arena_size = 1 * 1024 * 1024;
	thread_local byte thread_arena_memory[thread_arena_size];
	thread_local Arena arena = Arena::create(Slice(&thread_arena_memory[0], thread_arena_size));
	return &arena;
}

int main(){
	auto buf = thread_arena()->make<byte>(512);
	auto nums = thread_arena()->make<f64>(30);
	for(isize i = 0; i < nums.len(); i++)
		nums[i] = i / 2.0;

	print("Nums:", nums, "Skibidi");
	// std::cout << into_string(nums, buf).unwrap() << '\n';
}

