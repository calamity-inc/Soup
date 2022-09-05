#include "base.hpp"

#if SOUP_WASM || SOUP_CODE_INSPECTOR

#include "Canvas.hpp"
#include "QrCode.hpp"

using namespace soup;

static std::string ret_str_buf{};

[[nodiscard]] static const char* ret_str(std::string&& str)
{
	ret_str_buf = std::move(str);
	return ret_str_buf.c_str();
}

#define returnString(x) return ret_str(x);

// QrCode

SOUP_CEXPORT QrCode* QrCode_encodeText(const char* x)
{
	return new QrCode(QrCode::encodeText(x));
}

SOUP_CEXPORT void QrCode_free(QrCode* x)
{
	delete x;
}

SOUP_CEXPORT const char* QrCode_toSvg(QrCode* x, unsigned int border, bool black_bg, size_t scale)
{
	returnString(x->toCanvas(border, black_bg).toSvg(scale));
}

#endif
