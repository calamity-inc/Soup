#include "base32.hpp"

#include <algorithm>

namespace soup
{
	static unsigned char b32_alpha[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

	static unsigned char encode_char(unsigned char c)
	{
		return b32_alpha[c & 0b11111];
	}

	static int decode_char(unsigned char c)
	{
		if (c >= 'A' && c <= 'Z')
		{
			return c - 'A';
		}
		if (c >= '2' && c <= '7')
		{
			return c - '2' + 26;
		}
		return -1;
	}

	static int get_octet(int block)
	{
		return (block * 5) / 8;
	}

	static int get_offset(int block)
	{
		return (8 - 5 - (5 * block) % 8);
	}

	static unsigned char shift_right(unsigned char byte, char offset)
	{
		if (offset < 0)
		{
			return byte << -offset;
		}
		return byte >> offset;
	}

	static unsigned char shift_left(unsigned char byte, char offset)
	{
		return shift_right(byte, -offset);
	}

	static void encode_sequence(const uint8_t* plain, size_t len, std::string& out)
	{
		for (int block = 0; block != 8; ++block)
		{
			int octet = get_octet(block);  // figure out which octet this block starts in
			int junk = get_offset(block);  // how many bits do we drop from this octet?

			if (octet >= len) // we hit the end of the buffer
			{
				out.append(8 - block, '=');
				return;
			}

			unsigned char c = shift_right(plain[octet], junk);  // first part

			if (junk < 0  // is there a second part?
				&& octet < len - 1)  // is there still something to read?
			{
				c |= shift_right(plain[octet + 1], 8 + junk);
			}
			out.push_back(encode_char(c));
		}
	}

	std::string base32::encode(const std::string& in)
	{
		const auto len = in.length();
		std::string out{};
		out.reserve(getEncodedLength(len));
		for (size_t i = 0; i < len; i += 5)
		{
			encode_sequence((const uint8_t*)&in.at(i), std::min<size_t>(len - i, 5), out);
		}
		return out;
	}

	static bool decode_sequence(const uint8_t* coded, uint8_t octet_base, std::string& out)
	{
		for (int block = 0; block != 8; ++block)
		{
			int offset = get_offset(block);
			int octet = octet_base + get_octet(block);

			int c = decode_char(coded[block]);
			if (c == -1)
			{
				return false;
			}

			if (out.size() == octet)
			{
				out.push_back(shift_left(c, offset));
			}
			else
			{
				/*if (octet > out.size())
				{
					out.append(octet - out.size(), 0);
				}*/
				out.at(octet) |= shift_left(c, offset);
			}

			if (offset < 0) // does this block overflow to next octet?
			{
				auto overflow = shift_left(c, 8 + offset);
				if (overflow != 0)
				{
					out.push_back(overflow);
				}
			}
		}
		return true;
	}

	std::string base32::decode(const std::string& in)
	{
		auto arr = (const uint8_t*)&in.at(0);
		std::string out;
		out.reserve(getDecodedLength(in.size()));
		for (size_t i = 0, j = 0; ; i += 8, j += 5)
		{
			if (!decode_sequence(&arr[i], j, out))
			{
				break;
			}
		}
		return out;
	}
}
