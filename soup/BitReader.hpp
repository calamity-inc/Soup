#pragma once

#include <cstdint>

#include "Reader.hpp"

NAMESPACE_SOUP
{
	class BitReader
	{
	public:
		Reader* r;
		uint8_t bit_idx = 0;
		uint8_t byte;

		BitReader(Reader* r)
			: r(r)
		{
		}

		[[nodiscard]] Reader& getStream() const noexcept
		{
			return *r;
		}

		[[nodiscard]] constexpr bool isByteAligned() const noexcept
		{
			return bit_idx == 0;
		}

		[[nodiscard]] bool hasMore() const noexcept
		{
			return !isByteAligned() || getStream().hasMore();
		}

		bool finishByte() noexcept;

	protected:
		void ignore(uint8_t bits);
		bool forward(uint8_t bits);

	public:
		template <typename T>
		bool t(uint8_t bits, T& out) // out needs to be initialised before calling
		{
			if (bits == 1)
			{
				bool tmp;
				if (!b(tmp))
				{
					return false;
				}
				out = tmp;
				return true;
			}
			while (bits >= 8)
			{
				uint8_t tmp;
				if (!u8(bits, tmp))
				{
					return false;
				}
				bits -= 8;
				out <<= 8;
				out |= tmp;
			}
			uint8_t tmp;
			if (!u8(bits, tmp))
			{
				return false;
			}
			out <<= bits;
			out |= tmp;
			return true;
		}

		bool b(bool& out);
		bool u8(uint8_t bits, uint8_t& out);
		bool u16_dyn_2(uint16_t& val); // A 16-bit value encoded using 5-18 bits. Assumes that smaller numbers are the norm.
		bool u17_dyn_2(uint32_t& val); // A 17-bit value encoded using 5-19 bits. Assumes that smaller numbers are the norm.
		bool u20_dyn(uint32_t& val); // A 20-bit value encoded using 6-22 bits. Assumes that smaller numbers are the norm.
		bool u32_dyn(uint32_t& val); // A 32-bit value encoded using 10-34 bits. Assumes that smaller numbers are the norm.
		bool str_utf8_nt(std::string& str); // A UTF-8 string encoded using 6-23 bits per codepoint. Null-terminated.
		bool str_utf32_nt(std::u32string& str); // A UTF-32 string encoded using 6-23 bits per codepoint. Null-terminated.
		bool str_utf8_lp(std::string& str, uint8_t lpbits); // A UTF-8 string encoded using 6-23 bits per codepoint. Length-prefixed.
		bool str_utf32_lp(std::u32string& str, uint8_t lpbits); // A UTF-32 string encoded using 6-23 bits per codepoint. Length-prefixed.
	};
}
