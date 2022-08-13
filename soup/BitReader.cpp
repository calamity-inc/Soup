#include "BitReader.hpp"

#define DEBUG_BR false

#if DEBUG_BR
#include <iostream>
#endif

namespace soup
{
	bool BitReader::finishByte() noexcept
	{
		bit_idx = 0;
		return true;
	}

	void BitReader::ignore(uint8_t bits)
	{
		if (isByteAligned())
		{
			r->u8(byte);
		}
		forward(bits);
	}

	bool BitReader::forward(uint8_t bits)
	{
		bits %= 8;
		bit_idx += bits;
		if (bit_idx == 8)
		{
			bit_idx = 0;
		}
		else if (bit_idx > 8)
		{
			bit_idx -= 8;
			if (!r->u8(byte))
			{
				bit_idx = 0;
				return false;
			}
		}
		return true;
	}

	bool BitReader::b(bool& out)
	{
		if (isByteAligned())
		{
			bit_idx = 1;
			if (!r->u8(byte))
			{
				return false;
			}
			out = byte & 1;
		}
		else
		{
			out = (byte >> bit_idx) & 1;
			if (++bit_idx == 8)
			{
				bit_idx = 0;
			}
		}
		return true;
	}

	bool BitReader::u8(uint8_t bits, uint8_t& out)
	{
		if (isByteAligned())
		{
			bit_idx = bits;
			if (!r->u8(byte))
			{
				bit_idx = 0;
				return false;
			}
#if DEBUG_BR
			std::cout << "Fetched byte: " << std::hex << (int)byte << std::endl;
#endif
			out = (byte & ((1 << bits) - 1));
#if DEBUG_BR
			std::cout << "Taking " << std::dec << (int)bits << " bits: " << std::hex << (int)out << std::endl;
#endif
		}
		else
		{
			if (bit_idx + bits > 8)
			{
				out = (byte >> bit_idx);

				auto remaining_bits = (8 - bit_idx);
				auto next_byte_bits = (bit_idx - (8 - bits));
#if DEBUG_BR
				std::cout << "We want to take " << std::dec << (int)bits << " bits at bit_idx " << (int)bit_idx << ", so we'll take the remaining " << (int)remaining_bits << " bits and go to the next byte for " << (int)next_byte_bits << " more bits" << std::endl;
#endif
				out <<= next_byte_bits;
				if (forward(remaining_bits))
				{
					uint8_t next_byte;
					if (u8(next_byte_bits, next_byte))
					{
						out |= next_byte;
					}
				}
			}
			else
			{
				out = ((byte >> bit_idx) & ((1 << bits) - 1));
				forward(bits);
			}
		}
		return true;
	}
}
