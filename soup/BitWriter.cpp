#include "BitWriter.hpp"

#include "unicode.hpp"

NAMESPACE_SOUP
{
	bool BitWriter::commitByte()
	{
		bool ret = w->u8(byte);
		bit_idx = 0;
		byte = 0;
		return ret;
	}

	bool BitWriter::b(bool val)
	{
		/*if (bit_idx >= 8)
		{
			throw 0;
		}*/
		//std::cout << "Encoded b " << (val ? "true" : "false") << "\n";
		byte |= (val << bit_idx);
		++bit_idx;
		return bit_idx != 8 || commitByte();
	}

	bool BitWriter::u8(uint8_t bits, uint8_t val)
	{
		/*if (bits == 0)
		{
			return true;
		}*/

		if (bits > 8)
		{
			bits = 8;
		}

		val &= ((1 << bits) - 1);

		auto bufferable_bits = (8 - bit_idx);
		auto next_byte_bits = (bit_idx - (8 - bits));
		if (next_byte_bits < 0)
		{
			next_byte_bits = 0;
		}

		if (bits <= bufferable_bits)
		{
			//std::cout << "We want to write " << (int)bits << " bits (" << (int)val << ") at " << (int)bit_idx << ", we can buffer " << (int)bufferable_bits << "\n";
			byte |= (val << bit_idx);
			bit_idx += bits;
			return bit_idx < 8 || commitByte();
		}

		//std::cout << "We want to write " << (int)bits << " bits (" << (int)val << ") at " << (int)bit_idx << " so we'll have to write " << (int)(val >> next_byte_bits) << " and " << (int)next_byte_bits << " bits to the next one.\n";

		byte |= ((val >> next_byte_bits) << bit_idx);

		return commitByte()
			&& u8(next_byte_bits, val)
			;
	}

	bool BitWriter::u20_dyn(uint32_t val)
	{
		uint8_t nibbles_needed = 0;
		if (val <= 0xF)
		{
			nibbles_needed = 1;
		}
		else if (val <= 0xFF)
		{
			nibbles_needed = 2;
		}
		else if (val <= 0xFFF)
		{
			nibbles_needed = 3;
		}

		if (!u8(2, nibbles_needed))
		{
			return false;
		}

		return nibbles_needed == 0
			? t(20, val)
			: t(nibbles_needed * 4, val)
			;
	}
}
