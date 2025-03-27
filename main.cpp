#include "core/core.hpp"
#include "core/memory.hpp"
#include "core/atomic.hpp"
#include "core/format.hpp"
#include "core/dynamic_array.hpp"
#include "core/string_builder.hpp"

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

namespace foo {
struct Person {
	char const* name;
};

core::String format(core::FormattingContext* ctx, Person const& p){
	return "<>";
}
}

#define STB_SPRINTF_IMPLEMENTATION
#include "core/wip/stb_sprintf.h"

using namespace core;


Arena* thread_arena(){
	constexpr isize thread_arena_size = 1 * 1024 * 1024;
	thread_local byte thread_arena_memory[thread_arena_size];
	thread_local Arena arena = Arena::create(Slice(&thread_arena_memory[0], thread_arena_size));
	return &arena;
}

#include <thread>
#include <mutex>

int main(){
	auto buf = thread_arena()->make<byte>(512);

	auto mtx = new std::mutex();

	auto arr = DynamicArray<std::thread>::create(heap_allocator(), 10);
	for(isize i = 0; i < 10; i++){
		arr.append(std::thread([=](){
			mtx->lock();
			defer(mtx->unlock());

			auto reg = thread_arena()->create_region();
			defer(reg.release());

			auto p = thread_arena()->make<i32>(64);
			std::cout << i << "|" << p.data() << ": " << thread_arena()->offset << '\n';
		}));
	}

	for(auto& t : arr){
		t.join();
	}

	// auto s = format(buf, true);
	// std::cout << s << "...\n";
	auto arena = Arena::create(buf);
	auto sb = StringBuilder::create(&arena);

	sb.append("Hello");
	std::cout << sb.build() << '\n';
	std::cout << sb.build() << '\n';

	// using namespace kielo;
	// String src =
	// 	"fn main(){\n"
	// 	"  let x: f32 = (6 + 4) / 2;\n"
	// 	"}\n"
	// ;

	// auto lex = Lexer::create(src);

	// for(auto token = lex.next().or_else({});
	// 	token.type != TokenType::EndOfFile;
	// 	token = lex.next().or_else({}))
	// {
	// 	std::cout << token << std::endl;
	// }
}

