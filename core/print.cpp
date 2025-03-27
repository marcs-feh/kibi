#include "print.hpp"
#include "format.hpp"
#include "string_builder.hpp"

namespace core {
thread_local byte print_arena_buf[print_arena_size];
thread_local auto print_arena = Arena::create(Slice<byte>(&print_arena_buf[0], print_arena_size));

Arena* print_arena_get(){ return &print_arena; }
}
