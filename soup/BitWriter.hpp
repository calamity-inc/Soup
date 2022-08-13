#pragma once

#include <cstdint>

#include "Writer.hpp"

namespace soup
{
	struct BitWriter
	{
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
	};
}
