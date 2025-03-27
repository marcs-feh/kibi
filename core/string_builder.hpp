#pragma once
#include "core.hpp"
#include "memory.hpp"
#include "dynamic_array.hpp"

namespace core {

struct StringBuilder {
    byte* data;
	isize len;
	isize cap;
	Allocator* allocator;

    static constexpr isize default_capacity = 16;

	bool resize(isize new_cap){
		auto p = allocator->realloc(data, new_cap, len, alignof(byte));
		if(p == nullptr){ return false; }
		data = (byte*)p;
		cap = new_cap;
		len = min(len, new_cap);
		return true;
	}

	void expand(isize new_len){
		ensure(new_len >= len, "Not expanding");
		if(new_len >= cap){
			if(!resize(new_len)){ return; }
		}
		len = new_len;
	}

    isize append(Slice<byte> b){
		if((len + b.len()) >= cap){
			auto ok = resize(max<isize>(16, len * 2));
			if(!ok){ return 0; }
		}

		mem_copy_no_overlap(&data[len], b.data(), b.len());
		len += b.len();
		return b.len();
	}

	isize append(String s){
		return append(s.raw_bytes());
	}

	StringBuilder* drop(){
		allocator->free(data, cap, alignof(byte));
		return this;
	}

	void pop(isize count){
		len = max(len - count, isize(0));
	}

	isize append(byte b){
		if(len >= cap){
			auto ok = resize(max<isize>(16, len * 2));
			if(!ok){ return 0; }
		}
		len += 1;
		data[len] = b;
		return 1;
	}

	[[nodiscard]]
    String build(){
		resize(len);
		auto bytes = Slice<byte>(data, len);
		data = nullptr;
		len = 0;
		cap = 0;
        return String::from_bytes(bytes);
    }

    String build_copy(Allocator* allocator){
		auto out = allocator->make<byte>(len);
		if(out.data() != nullptr){
			mem_copy_no_overlap(out.data(), data, len);
		}
		return String::from_bytes(out);
	}
    
    [[nodiscard]]
    static StringBuilder create(Allocator* allocator, isize initial_cap = default_capacity){
        StringBuilder sb;
		sb.allocator = allocator;
        sb.data = (byte*)allocator->alloc(initial_cap, alignof(byte));
        return sb;
    }

	Slice<byte> slice(){
		return Slice<byte>(data, len);
	}

    StringBuilder()
		: data{nullptr}
		, len{0}
		, cap{0}
		, allocator{nullptr} {}

    StringBuilder(StringBuilder&& sb)
        : data{core::exchange(sb.data, nullptr)}
		, len{core::exchange(sb.len, 0)}
		, cap{core::exchange(sb.cap, 0)}
		, allocator{core::exchange(sb.allocator, nullptr)} {}
    
    StringBuilder& operator=(StringBuilder&& sb){
        return *new (drop()) StringBuilder{ core::move(sb) };
    }

    ~StringBuilder(){ drop(); }

};
}
