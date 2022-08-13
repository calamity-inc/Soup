#include "BitWriter.hpp"

namespace soup
{
	bool BitWriter::finishByte()
	{
		bool ret = w->u8(byte);
		bit_idx = 0;
		byte = 0;
		return ret;
	}

	bool BitWriter::b(bool val)
	{
		byte |= (val << bit_idx);
		++bit_idx;
		return bit_idx != 8 || finishByte();
	}

	bool BitWriter::u8(uint8_t bits, uint8_t val)
	{
		auto curr_byte_bits = (8 - bit_idx);
		auto next_byte_bits = (bit_idx - (8 - bits));

		byte |= (val << bit_idx);
		bit_idx += bits;
		return bit_idx < 8
			|| (finishByte()
				&& u8(next_byte_bits, val >> curr_byte_bits)
				)
			;
	}
}
