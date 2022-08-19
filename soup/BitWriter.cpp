#include "BitWriter.hpp"

#include "unicode.hpp"

namespace soup
{
	bool BitWriter::finishByte()
	{
		return bit_idx == 0 || commitByte();
	}

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
			//std::cout << "We want to write " << (int)bits << " at " << (int)bit_idx << ", we can buffer " << (int)bufferable_bits << "\n";
			byte |= (val << bit_idx);
			bit_idx += bits;
			return bit_idx < 8 || commitByte();
		}

		//std::cout << "We want to write " << (int)bits << " at " << (int)bit_idx << " so we'll have to write " << (int)next_byte_bits << " bits to the next one.\n";

		byte |= ((val >> next_byte_bits) << bit_idx);

		return commitByte()
			&& u8(next_byte_bits, val)
			;
	}

	bool BitWriter::u16_dyn(uint16_t val)
	{
		if (val <= 0xFF)
		{
			return b(true)
				&& u8(8, val)
				;
		}
		return b(false)
			&& t(16, val)
			;
	}

	bool BitWriter::u16_dyn_2(uint16_t val)
	{
		uint8_t nibbles_needed = 3;
		if (val <= 0xF)
		{
			nibbles_needed = 0;
		}
		else if (val <= 0xFF)
		{
			nibbles_needed = 1;
		}
		else if (val <= 0xFFF)
		{
			nibbles_needed = 2;
		}
		return u8(2, nibbles_needed)
			&& t(((nibbles_needed + 1) * 4), val)
			;
	}

	bool BitWriter::u17_dyn_2(uint32_t val)
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
			? t(17, val)
			: t(nibbles_needed * 4, val)
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

	bool BitWriter::u32_dyn(uint32_t val)
	{
		uint8_t bytes_needed = 3;
		if (val <= 0xFF)
		{
			bytes_needed = 0;
		}
		else if (val <= 0xFFFF)
		{
			bytes_needed = 1;
		}
		else if (val <= 0xFFFFFF)
		{
			bytes_needed = 2;
		}
		//std::cout << "Encoding u32 " << val << ", bytes_needed = " << (int)bytes_needed << "\n";
		return u8(2, bytes_needed)
			&& t((bytes_needed + 1) * 8, val)
			;
	}

	bool BitWriter::str_utf8_nt(const std::string& str)
	{
		return str_utf32_nt(unicode::utf8_to_utf32(str));
	}

	bool BitWriter::str_utf32_nt(const std::u32string& str)
	{
		return str_utf32_raw(str)
			&& b(false)
			&& u20_dyn(0)
			;
	}

	bool BitWriter::str_utf8_lp(const std::string& str, uint8_t lpbits)
	{
		return str_utf32_lp(unicode::utf8_to_utf32(str), lpbits);
	}

	bool BitWriter::str_utf32_lp(const std::u32string& str, uint8_t lpbits)
	{
		const auto lpmask = ((1 << lpbits) - 1);
		if (str.length() < lpmask)
		{
			if (!t(lpbits, str.length()))
			{
				return false;
			}
		}
		else
		{
			if (!t(lpbits, lpmask)
				|| !u32_dyn(str.length())
				)
			{
				return false;
			}
		}
		return str_utf32_raw(str);
	}

	bool BitWriter::str_utf32_raw(const std::u32string& str)
	{
		const std::string charset = "abcdefghijklmnopqrstuvwxyzTIAHSW";
		for (char32_t c : str)
		{
			if (c <= 0xFF)
			{
				auto idx = charset.find((char)c);
				if (idx != std::string::npos)
				{
					if (!b(true)
						|| !u8(5, idx)
						)
					{
						return false;
					}
					continue;
				}
			}
			if (c > 'z')
			{
				c -= 26;
			}
			if (!b(false)
				|| !u20_dyn(c)
				)
			{
				return false;
			}
		}
		return true;
	}
}
