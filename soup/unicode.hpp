#pragma once

#include <string>

#include "base.hpp"

#define UTF8_CONTINUATION_FLAG 0b10000000
#define UTF8_HAS_CONTINUATION(ch) (ch & 0b10000000)
#define UTF8_IS_CONTINUATION(ch) ((ch & 0b11000000) == UTF8_CONTINUATION_FLAG)

#if SOUP_WINDOWS
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
		static constexpr uint32_t REPLACEMENT_CHAR = 0xFFFD;

		[[nodiscard]] static char32_t utf8_to_utf32_char(std::string::const_iterator& it, const std::string::const_iterator end) noexcept;
#if SOUP_CPP20
		[[nodiscard]] static std::u32string utf8_to_utf32(const char8_t* utf8) noexcept;
#endif
		[[nodiscard]] static std::u32string utf8_to_utf32(const std::string& utf8) noexcept;
#if SOUP_CPP20
		[[nodiscard]] static UTF16_STRING_TYPE utf8_to_utf16(const char8_t* utf8) noexcept;
#endif
		[[nodiscard]] static UTF16_STRING_TYPE utf8_to_utf16(const std::string& utf8) noexcept;
		[[nodiscard]] static UTF16_STRING_TYPE utf32_to_utf16(const std::u32string& utf32) noexcept;
		[[nodiscard]] static std::string utf32_to_utf8(char32_t utf32) noexcept;
		[[nodiscard]] static std::string utf32_to_utf8(const std::u32string& utf32) noexcept;

		template <typename Str = std::u16string>
		[[nodiscard]] static char32_t utf16_to_utf32(typename Str::const_iterator& it, const typename Str::const_iterator end) noexcept
		{
			if ((*it & 0b1111110000000000) == 0b1101100000000000)
			{
				auto hi = (char32_t)(*it & 0b0000001111111111);
				if (++it == end)
				{
					return 0;
				}
				auto lo = (char32_t)(*it++ & 0b0000001111111111);
				return (((hi * 0x400) + lo) + 0x10000);
			}
			return (char32_t)*it++;
		}

		template <typename Str>
		[[nodiscard]] static std::u32string utf16_to_utf32(const Str& utf16)
		{
			std::u32string utf32{};
			auto it = utf16.cbegin();
			const auto end = utf16.cend();
			while (it != end)
			{
				auto uni = utf16_to_utf32<Str>(it, end);
				if (uni == 0)
				{
					utf32.push_back(REPLACEMENT_CHAR);
				}
				else
				{
					utf32.push_back(uni);
				}
			}
			return utf32;
		}

		template <typename Str>
		[[nodiscard]] static std::string utf16_to_utf8(const Str& utf16)
		{
			return utf32_to_utf8(utf16_to_utf32(utf16));
		}

		[[nodiscard]] static size_t utf8_char_len(const std::string& str) noexcept;
	};
}
