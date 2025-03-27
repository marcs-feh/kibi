#pragma once
#include "core.hpp"
#include "memory.hpp"
#include "dynamic_array.hpp"

namespace core {

struct StringBuilder {
    DynamicArray<byte> buf;

    static constexpr isize default_capacity = 16;

    isize append(String s){
		auto old_len = buf.len();
		buf.append(s.raw_bytes());
		return buf.len() - old_len;
	}

    isize append(rune r){
		auto [bytes, n] = utf8_encode(r);
		auto s = Slice<byte>(&bytes[0], n);
		auto old_len = buf.len();
		buf.append(s);
		return buf.len() - old_len;
	}

    isize append(byte b){
		auto old_len = buf.len();
		buf.append(b);
		return buf.len() - old_len;
	}

    isize append(Slice<byte> b){
		auto old_len = buf.len();
		buf.append(b);
		return buf.len() - old_len;
	}

	[[nodiscard]]
    String build(){
        auto bytes = buf.get_owned_slice();
        return String::from_bytes(bytes);
    }

    String build_copy(Allocator* allocator){
		auto out = allocator->make<byte>(buf.len());
		if(out.data() != nullptr){
			mem_copy_no_overlap(out.data(), buf.data(), buf.len());
		}
		return String::from_bytes(out);
	}
    
    [[nodiscard]]
    static StringBuilder create(Allocator* allocator, isize initial_cap = default_capacity){
        StringBuilder sb;
        sb.buf = DynamicArray<byte>::create(allocator, initial_cap);
        return sb;
    }

    StringBuilder copy(Allocator* allocator){
        StringBuilder sb;
        sb.buf = buf.copy(allocator);
        return sb;
    }

    StringBuilder() : buf{} {}

	StringBuilder* drop(){
        buf.drop();
        return this;
    }

    StringBuilder(StringBuilder&& sb)
        : buf{core::move(sb.buf)} {}
    
    StringBuilder& operator=(StringBuilder&& sb){
        return *new (drop()) StringBuilder{ core::move(sb) };
    }

    ~StringBuilder(){ drop(); }
};
};
