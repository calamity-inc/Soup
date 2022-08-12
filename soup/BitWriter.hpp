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

		[[nodiscard]] constexpr bool isByteAligned() const noexcept
		{
			return bit_idx == 0;
		}

		void flush()
		{
			w->u8(byte);
			bit_idx = 0;
			byte = 0;
		}

		template <uint8_t Bits, typename T>
		void write(T val)
		{
			if constexpr (Bits == 1)
			{
				writeBit(val);
			}
			else
			{
				writeByte(Bits, val);
			}
		}

		void writeBit(bool val)
		{
			byte |= (val << bit_idx);
			++bit_idx;
			if (bit_idx == 8)
			{
				flush();
			}
		}

		void writeByte(uint8_t bits, uint8_t val)
		{
			auto curr_byte_bits = (8 - bit_idx);
			auto next_byte_bits = (bit_idx - (8 - bits));

			byte |= (val << bit_idx);
			bit_idx += bits;
			if (bit_idx >= 8)
			{
				flush();
				writeByte(next_byte_bits, val >> curr_byte_bits);
			}
		}
	};
}
