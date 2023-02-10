#include "OpaqueString.hpp"

namespace soup
{
	OpaqueString::OpaqueString(const char* str) noexcept
		: data(unicode::utf8_to_utf32(str))
	{
	}

#if SOUP_CPP20
	OpaqueString::OpaqueString(const char8_t* str) noexcept
		: data(unicode::utf8_to_utf32(str))
	{
	}
#endif

	OpaqueString::OpaqueString(const std::string& str) noexcept
		: data(unicode::utf8_to_utf32(str))
	{
	}

	void OpaqueString::operator=(const char* str) noexcept
	{
		data = unicode::utf8_to_utf32(str);
	}

#if SOUP_CPP20
	void OpaqueString::operator=(const char8_t* str) noexcept
	{
		data = unicode::utf8_to_utf32(str);
	}
#endif

	void OpaqueString::operator=(const std::string& str) noexcept
	{
		data = unicode::utf8_to_utf32(str);
	}

	OpaqueString::operator std::string() const noexcept
	{
		return unicode::utf32_to_utf8(data);
	}

	OpaqueString::OpaqueString(const UTF16_CHAR_TYPE* str) noexcept
		: data(unicode::utf16_to_utf32<UTF16_STRING_TYPE>(str))
	{
	}

	OpaqueString::OpaqueString(const UTF16_STRING_TYPE& str) noexcept
		: data(unicode::utf16_to_utf32(str))
	{
	}

	void OpaqueString::operator=(const UTF16_CHAR_TYPE* str) noexcept
	{
		data = unicode::utf16_to_utf32<UTF16_STRING_TYPE>(str);
	}

	void OpaqueString::operator=(const UTF16_STRING_TYPE& str) noexcept
	{
		data = unicode::utf16_to_utf32(str);
	}

	OpaqueString::operator UTF16_STRING_TYPE() const noexcept
	{
		return unicode::utf32_to_utf16(data);
	}

	OpaqueString::OpaqueString(std::u32string str) noexcept
		: data(std::move(str))
	{
	}

	void OpaqueString::operator=(std::u32string str) noexcept
	{
		data = std::move(str);
	}

	OpaqueString::operator std::u32string& () noexcept
	{
		return data;
	}

	OpaqueString::operator const std::u32string& () const noexcept
	{
		return data;
	}

	OpaqueString OpaqueString::substr(size_t i, size_t l)
	{
		return data.substr(i, l);
	}

	std::ostream& operator<<(std::ostream& os, const OpaqueString& v)
	{
		os << v.operator std::string();
		return os;
	}
}
