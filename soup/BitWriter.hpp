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

		template <uint8_t Bits, typename T>
		bool t(T val)
		{
			if constexpr (Bits == 1)
			{
				return b(val);
			}
			return u8(Bits, val);
		}

		bool b(bool val);
		bool u8(uint8_t bits, uint8_t val);
	};
}
