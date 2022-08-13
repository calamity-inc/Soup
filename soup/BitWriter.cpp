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
		byte |= (val << bit_idx);
		++bit_idx;
		return bit_idx != 8 || commitByte();
	}

	bool BitWriter::u8(uint8_t bits, uint8_t val)
	{
		val &= ((1 << bits) - 1);

		auto bufferable_bits = (8 - bit_idx);
		auto next_byte_bits = (bit_idx - (8 - bits));

		if (bits <= bufferable_bits)
		{
			byte |= (val << bit_idx);
			bit_idx += bits;
			return bit_idx < 8 || commitByte();
		}
		
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

		if (nibbles_needed == 0)
		{
			if (!u8(2, 0b11)
				|| !t(20, val)
				)
			{
				return false;
			}
		}
		else
		{
			if (!u8(2, nibbles_needed))
			{
				return false;
			}
			while (nibbles_needed--)
			{
				if (!u8(4, val))
				{
					return false;
				}
				val >>= 4;
			}
		}
		return true;
	}

	bool BitWriter::u32_dyn(uint32_t val)
	{
		uint8_t bytes_needed = 4;
		if (val <= 0xFF)
		{
			bytes_needed = 1;
		}
		else if (val <= 0xFFFF)
		{
			bytes_needed = 2;
		}
		else if (val <= 0xFFFFFF)
		{
			bytes_needed = 3;
		}

		if (!u8(2, bytes_needed - 1))
		{
			return false;
		}

		while (bytes_needed--)
		{
			if (!u8(8, val))
			{
				return false;
			}
			val >>= 8;
		}
		return true;
	}

	bool BitWriter::str_utf8dyn(const std::string& str)
	{
		return str_utf32dyn(unicode::utf8_to_utf32(str));
	}

	bool BitWriter::str_utf32dyn(const std::u32string& str)
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
		if (!b(false)
			|| !u20_dyn(0)
			)
		{
			return false;
		}
		return true;
	}
}
