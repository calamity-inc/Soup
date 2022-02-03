#pragma once

#include <string>

#include "platform.hpp"

#define UTF8_CONTINUATION_FLAG 0b10000000
#define UTF8_IS_CONTINUATION(ch) ((ch & 0b11000000) == UTF8_CONTINUATION_FLAG)

#if SOUP_PLATFORM_WINDOWS
static_assert(sizeof(wchar_t) == 2);
#define UTF16_CHAR_TYPE wchar_t
#define UTF16_STRING_TYPE std::wstring
#else
#define UTF16_CHAR_TYPE char16_t
#define UTF16_STRING_TYPE std::u16string
#endif

namespace soup
{
	struct unicode
	{
		static constexpr uint32_t unicode_replacement_char = 0xFFFD;

		[[nodiscard]] static char32_t utf8_to_utf32_char(std::string::const_iterator& it, const std::string::const_iterator end) noexcept;
		[[nodiscard]] static std::u32string utf8_to_utf32(const std::string& utf8) noexcept;
		[[nodiscard]] static UTF16_STRING_TYPE utf8_to_utf16(const std::string& utf8) noexcept;
		[[nodiscard]] static UTF16_STRING_TYPE utf32_to_utf16(const std::u32string& utf32) noexcept;
		[[nodiscard]] static std::string utf32_to_utf8(char32_t utf32) noexcept;
		[[nodiscard]] static std::string utf32_to_utf8(const std::u32string& utf32) noexcept;
	};
}
