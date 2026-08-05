#pragma once

#include <cstdint>

#include "Writer.hpp"

NAMESPACE_SOUP
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

		[[nodiscard]] Writer& getStream() const noexcept
		{
			return *w;
		}

		[[nodiscard]] constexpr bool isByteAligned() const noexcept
		{
			return bit_idx == 0;
		}

		bool finishByte()
		{
			return isByteAligned() || commitByte();
		}

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
			while (bits > 8)
			{
				if (!u8(bits, static_cast<uint8_t>(val >> (bits - 8))))
				{
					return false;
				}
				bits -= 8;
			}
			return u8(bits, static_cast<uint8_t>(val));
		}

		bool b(bool val);
		bool u8(uint8_t bits, uint8_t val);
		[[deprecated]] bool u20_dyn(uint32_t val); // A 20-bit value encoded using 6-22 bits. Assumes that smaller numbers are the norm.
	};
}
