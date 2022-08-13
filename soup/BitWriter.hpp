#pragma once

#include <cstdint>

#include "Writer.hpp"

namespace soup
{
	class BitWriter
	{
	public:
		Writer* w;
		uint8_t bit_idx = 0;
		uint8_t byte = 0;

		BitWriter(Writer* w)
			: w(w)
		{
		}

		[[nodiscard]] Writer* getStream() const noexcept
		{
			return w;
		}

		[[nodiscard]] constexpr bool isByteAligned() const noexcept
		{
			return bit_idx == 0;
		}

		bool finishByte();
	protected:
		bool commitByte();

	public:
		template <typename T>
		bool t(uint8_t bits, T val)
		{
			if (bits == 1)
			{
				return b(val);
			}
			while (bits >= 8)
			{
				if (!u8(bits, val))
				{
					return false;
				}
				bits -= 8;
				val >>= 8;
			}
			return u8(bits, val);
		}

		bool b(bool val);
		bool u8(uint8_t bits, uint8_t val);
		bool u20_dyn(uint32_t val); // A 20-bit value encoded using up 6-22 bits.
		bool u32_dyn(uint32_t val); // A 32-bit value encoded using up 10-34 bits.
		bool str_utf8dyn(const std::string& str); // A UTF-8 string encoded using 6-23 bits per codepoint.
		bool str_utf32dyn(const std::u32string& str); // A UTF-32 string encoded using 6-23 bits per codepoint.
	};
}
