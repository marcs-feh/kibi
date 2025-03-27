#include "format.hpp"

namespace core {

String format(FormattingContext* ctx, f64 val){
	constexpr char const* float_fmt = "%*.*f";
	auto buf = ctx->buffer;

	isize len = stbsp_snprintf((char*)buf.data(), buf.len(), float_fmt, (int)ctx->left_pad, (int)ctx->precision, val);

	return String::from_bytes(buf[{0, len}]);
}

String format(FormattingContext* ctx, i64 const& val){
	bool pad_with_zero = u8(ctx->flags) & u8(FormattingFlag::PadWithZeros);

	constexpr char const* hex_fmt = "%*llx";
	constexpr char const* dec_fmt = "%*lld";
	constexpr char const* oct_fmt = "%*lld";
	constexpr char const* bin_fmt = "%*llb";

	constexpr char const* hex_fmt_z = "%0*llx";
	constexpr char const* dec_fmt_z = "%0*lld";
	constexpr char const* oct_fmt_z = "%0*lld";
	constexpr char const* bin_fmt_z = "%0*llb";
	char const* fmt = nullptr;

	if(pad_with_zero){
		switch (ctx->base) {
			case 16: fmt = hex_fmt_z; break;
			case 10: fmt = dec_fmt_z; break;
			case 8:  fmt = oct_fmt_z; break;
			case 2:  fmt = bin_fmt_z; break;
		}
	}
	else {
		switch (ctx->base) {
			case 16: fmt = hex_fmt; break;
			case 10: fmt = dec_fmt; break;
			case 8:  fmt = oct_fmt; break;
			case 2:  fmt = bin_fmt; break;
		}
	}

	isize len = 0;
	auto buf = ctx->buffer;
	if(fmt != nullptr){
		len = stbsp_snprintf((char*)buf.data(), buf.len(), fmt, (int)ctx->left_pad, (long long)val);
	}
	else {
		len = stbsp_snprintf((char*)buf.data(), buf.len(), "!< INVALID BASE: %d >", ctx->base);
	}
	return String::from_bytes(buf[{0, len}]);
}

String format(FormattingContext* ctx, bool val){
	char const* fmt = nullptr;
	isize len = 0;
	auto buf = ctx->buffer;

	if(val){
		fmt = "true";
		len = sizeof("true");
	} else {
		fmt = "false";
		len = sizeof("false");
	}

	len = min(buf.len(), len);
	mem_copy(buf.data(), fmt, len);
	return String::from_bytes(buf[{0, len}]);
}

String format(FormattingContext* ctx, String const& s){
	constexpr char const* str_fmt = "%*.*s";
	auto buf = ctx->buffer;

	isize len = stbsp_snprintf((char*)buf.data(), buf.len(), str_fmt, (int)ctx->left_pad, (int)s.len(), s.data());

	return String::from_bytes(buf[{0, len}]);
}
} /* namespace core */
