#pragma once

#include "core.hpp"

namespace core {

enum class StreamError : u32 {
    None = 0,

    BrokenChannel,
    Closed,
    Unsupported,
    EndOfStream,
};

enum class StreamMode : u8 {
    Read  = (1 << 0),
    Write = (1 << 1),
};

struct Stream {
    virtual Result<u64, StreamError> read(Slice<byte> buf) = 0;
    virtual Result<u64, StreamError> write(Slice<byte> buf) = 0;
    virtual StreamMode capabilities() = 0;
};

}