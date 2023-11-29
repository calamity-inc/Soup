#include "base58.hpp"

#include <vector>

#include "alpha2decodetbl.hpp"
#include "Exception.hpp"

/*
Original source: https://github.com/bitcoin/bitcoin/blob/master/src/base58.cpp
Original licence follows.

Copyright (c) 2009-2023 The Bitcoin Core developers
Copyright (c) 2009-2023 Bitcoin Developers

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

namespace soup
{
	std::string base58::decode(const std::string& in)
	{
		auto mapBase58 = alpha2decodetbl("123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz");

		const char* psz = in.c_str();

		// Skip and count leading '1's.
		int zeroes = 0;
		int length = 0;
		while (*psz == '1') {
			zeroes++;
			psz++;
		}
		// Allocate enough space in big-endian base256 representation.
		auto size = in.length() * 733 / 1000 + 1; // log(58) / log(256), rounded up.
		std::vector<uint8_t> b256(size);
		// Process the characters.
		while (*psz)
		{
			// Decode base58 character
			int carry = mapBase58[(uint8_t)*psz];
			if (carry == 0xFF)
			{
				SOUP_THROW(Exception("Invalid base58 character"));
			}
			int i = 0;
			for (std::vector<uint8_t>::reverse_iterator it = b256.rbegin(); (carry != 0 || i < length) && (it != b256.rend()); ++it, ++i) {
				carry += 58 * (*it);
				*it = carry % 256;
				carry /= 256;
			}
			length = i;
			psz++;
		}
		// Skip leading zeroes in b256.
		std::vector<uint8_t>::iterator it = b256.begin() + (size - length);
		// Copy result into output vector.
		std::string out;
		out.reserve(zeroes + (b256.end() - it));
		out.assign(zeroes, 0x00);
		while (it != b256.end())
		{
			out.push_back(*(it++));
		}
		return out;
	}
}
