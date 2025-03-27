#pragma once
#include "core.hpp"
#include "string_builder.hpp"

namespace core {

enum class FormattingFlag {
	PadWithZeros       = (1 << 0),
	ScientificNotation = (1 << 1),
	SeparateDigits     = (1 << 2),
};

struct FormattingContext {
	StringBuilder* buffer;
	i32 left_pad;
	i32 base;
	i32 max_width;
	i32 precision;
	u8 flags; /* Bitset of FormattingFlag */
};

template<typename T>
concept Formattable = requires(FormattingContext* ctx, T const& obj){
	{ format(ctx, obj) } -> ConvertibleTo<String>;
};

[[nodiscard]]
String format(FormattingContext* ctx, i64 const& val);

[[nodiscard]]
String format(FormattingContext* ctx, f64 val);

[[nodiscard]]
String format(FormattingContext* ctx, String const& s);

[[nodiscard]]
String format(FormattingContext* ctx, bool const& val);

} /* namespace core */
