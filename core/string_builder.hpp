#include "core.hpp"
#include "memory.hpp"
#include "dynamic_array.hpp"

namespace core {

// struct BuilderStream : Stream {};

struct StringBuilder {
    DynamicArray<byte> buf;

    static constexpr isize default_capacity = 16;

    bool append(String s);

    bool append(rune r);

    bool append(byte b);

    bool append(i64 n);
    
    bool append(f64 n);

    String build(){
        auto bytes = buf.get_owned_slice();
        return String::from_bytes(bytes);
    }

    String build_copy(Allocator* allocator);
    
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