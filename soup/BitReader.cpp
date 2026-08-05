#include "BitReader.hpp"

#include "unicode.hpp"

#define DEBUG_BR false

#if DEBUG_BR
#include <iostream>
#endif

NAMESPACE_SOUP
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
#if DEBUG_BR
			std::cout << "Fetched byte: " << std::hex << (int)byte << std::dec << std::endl;
#endif
		}
		return true;
	}

	bool BitReader::b(bool& out)
	{
		if (isByteAligned())
		{
			if (!r->u8(byte))
			{
				return false;
			}
			out = byte & 1;
			bit_idx = 1;
		}
		else
		{
			out = (byte >> bit_idx) & 1;
			if (++bit_idx == 8)
			{
				bit_idx = 0;
			}
		}
		//std::cout << "Decoded b " << (out ? "true" : "false") << "\n";
		return true;
	}

	bool BitReader::u8(uint8_t bits, uint8_t& out)
	{
		if (bits == 0)
		{
			out = 0;
			return true;
		}

		if (bits > 8)
		{
			bits = 8;
		}

		if (isByteAligned())
		{
			bit_idx = bits % 8;
			if (!r->u8(byte))
			{
				bit_idx = 0;
				return false;
			}
#if DEBUG_BR
			std::cout << "Fetched byte: " << std::hex << (int)byte << std::dec << std::endl;
#endif
			out = (byte & ((1 << bits) - 1));
		}
		else
		{
			if (bit_idx + bits > 8)
			{
				out = (byte >> bit_idx);

				auto remaining_bits = (8 - bit_idx);
				auto next_byte_bits = (bit_idx - (8 - bits));
#if DEBUG_BR
				std::cout << "We want to take " << (int)bits << " bits at bit_idx " << (int)bit_idx << ", so we'll take the remaining " << (int)remaining_bits << " bits (" << (int)out << ") and go to the next byte for " << (int)next_byte_bits << " more bits" << std::endl;
#endif
				out <<= next_byte_bits;
				if (forward(remaining_bits))
				{
					uint8_t next_byte;
					if (u8(next_byte_bits, next_byte))
					{
						out |= next_byte;
					}
#if DEBUG_BR
					else
					{
						std::cout << "Failed to read from next byte\n";
					}
#endif
				}
#if DEBUG_BR
				else
				{
					std::cout << "Failed to forward\n";
				}
#endif
			}
			else
			{
				out = ((byte >> bit_idx) & ((1 << bits) - 1));
				forward(bits);
			}
		}
#if DEBUG_BR
		std::cout << "Got " << (int)bits << " bits: " << std::hex << (int)out << std::dec << std::endl;
#endif
		return true;
	}

	bool BitReader::u20_dyn(uint32_t& val)
	{
		uint8_t nibbles_needed;
		if (!u8(2, nibbles_needed))
		{
			return false;
		}
		val = 0;
		if (nibbles_needed == 0
			? t(20, val)
			: t(nibbles_needed * 4, val)
			)
		{
			//std::cout << "Got u20_dyn: " << val << "\n";
			return true;
		}
		return false;
	}
}
