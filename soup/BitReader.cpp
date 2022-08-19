#include "BitReader.hpp"

#include "unicode.hpp"

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
		if (bits > 8)
		{
			bits = 8;
		}

		if (isByteAligned())
		{
			bit_idx = bits;
			if (!r->u8(byte))
			{
				return false;
			}
#if DEBUG_BR
			std::cout << "Fetched byte: " << std::hex << (int)byte << std::dec << std::endl;
#endif
			out = (byte & ((1 << bits) - 1));
#if DEBUG_BR
			std::cout << "Taking " << (int)bits << " bits: " << std::hex << (int)out << std::dec << std::endl;
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
				std::cout << "We want to take " << (int)bits << " bits at bit_idx " << (int)bit_idx << ", so we'll take the remaining " << (int)remaining_bits << " bits and go to the next byte for " << (int)next_byte_bits << " more bits" << std::endl;
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

	bool BitReader::u20_dyn(uint32_t& val)
	{
		uint8_t nibbles_needed;
		if (!u8(2, nibbles_needed))
		{
			return false;
		}

		val = 0;

		if (nibbles_needed == 0)
		{
			return t(20, val);
		}

		const auto total_nibbles_needed = nibbles_needed;
		while (nibbles_needed)
		{
			uint8_t tmp;
			if (!u8(4, tmp))
			{
				return false;
			}
			val |= (tmp << ((total_nibbles_needed - nibbles_needed) * 4));
			--nibbles_needed;
		}
		return true;
	}

	bool BitReader::str_utf8_nt(std::string& str)
	{
		std::u32string tmp{};
		bool ret = str_utf32_nt(tmp);
		str = unicode::utf32_to_utf8(tmp);
		return ret;
	}

	bool BitReader::str_utf32_nt(std::u32string& str)
	{
		const std::string charset = "abcdefghijklmnopqrstuvwxyzTIAHSW";
		while (true)
		{
			bool shorthand;
			if (!b(shorthand))
			{
				return false;
			}

			if (shorthand)
			{
				uint8_t tmp;
				if (!u8(5, tmp))
				{
					return false;
				}
				str.push_back(charset.at(tmp));
			}
			else
			{
				uint32_t tmp;
				if (!u20_dyn(tmp))
				{
					return false;
				}
				if (tmp == 0)
				{
					break;
				}
				if (tmp >= 'a')
				{
					tmp += 26;
				}
				str.push_back(tmp);
			}
		}
		return true;
	}
}
