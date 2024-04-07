#include "bech32.hpp"

/*
Original source: https://github.com/sipa/bech32
Original licence follows.

Copyright (c) 2017, 2021 Pieter Wuille

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

NAMESPACE_SOUP
{
	/** The Bech32 character set for encoding. */
	static const char* CHARSET = "qpzry9x8gf2tvdw0s3jn54khce6mua7l";

	/** Concatenate two byte arrays. */
	[[nodiscard]] static std::vector<uint8_t> cat(std::vector<uint8_t> x, const std::vector<uint8_t>& y)
	{
		x.insert(x.end(), y.begin(), y.end());
		return x;
	}

	/** This function will compute what 6 5-bit values to XOR into the last 6 input values, in order to
	 *  make the checksum 0. These 6 values are packed together in a single 30-bit integer. The higher
	 *  bits correspond to earlier values. */
	[[nodiscard]] static uint32_t polymod(const std::vector<uint8_t>& values)
	{
		// The input is interpreted as a list of coefficients of a polynomial over F = GF(32), with an
		// implicit 1 in front. If the input is [v0,v1,v2,v3,v4], that polynomial is v(x) =
		// 1*x^5 + v0*x^4 + v1*x^3 + v2*x^2 + v3*x + v4. The implicit 1 guarantees that
		// [v0,v1,v2,...] has a distinct checksum from [0,v0,v1,v2,...].

		// The output is a 30-bit integer whose 5-bit groups are the coefficients of the remainder of
		// v(x) mod g(x), where g(x) is the Bech32 generator,
		// x^6 + {29}x^5 + {22}x^4 + {20}x^3 + {21}x^2 + {29}x + {18}. g(x) is chosen in such a way
		// that the resulting code is a BCH code, guaranteeing detection of up to 3 errors within a
		// window of 1023 characters. Among the various possible BCH codes, one was selected to in
		// fact guarantee detection of up to 4 errors within a window of 89 characters.

		// Note that the coefficients are elements of GF(32), here represented as decimal numbers
		// between {}. In this finite field, addition is just XOR of the corresponding numbers. For
		// example, {27} + {13} = {27 ^ 13} = {22}. Multiplication is more complicated, and requires
		// treating the bits of values themselves as coefficients of a polynomial over a smaller field,
		// GF(2), and multiplying those polynomials mod a^5 + a^3 + 1. For example, {5} * {26} =
		// (a^2 + 1) * (a^4 + a^3 + a) = (a^4 + a^3 + a) * a^2 + (a^4 + a^3 + a) = a^6 + a^5 + a^4 + a
		// = a^3 + 1 (mod a^5 + a^3 + 1) = {9}.

		// During the course of the loop below, `c` contains the bitpacked coefficients of the
		// polynomial constructed from just the values of v that were processed so far, mod g(x). In
		// the above example, `c` initially corresponds to 1 mod g(x), and after processing 2 inputs of
		// v, it corresponds to x^2 + v0*x + v1 mod g(x). As 1 mod g(x) = 1, that is the starting value
		// for `c`.
		uint32_t c = 1;
		for (const auto v_i : values) {
			// We want to update `c` to correspond to a polynomial with one extra term. If the initial
			// value of `c` consists of the coefficients of c(x) = f(x) mod g(x), we modify it to
			// correspond to c'(x) = (f(x) * x + v_i) mod g(x), where v_i is the next input to
			// process. Simplifying:
			// c'(x) = (f(x) * x + v_i) mod g(x)
			//         ((f(x) mod g(x)) * x + v_i) mod g(x)
			//         (c(x) * x + v_i) mod g(x)
			// If c(x) = c0*x^5 + c1*x^4 + c2*x^3 + c3*x^2 + c4*x + c5, we want to compute
			// c'(x) = (c0*x^5 + c1*x^4 + c2*x^3 + c3*x^2 + c4*x + c5) * x + v_i mod g(x)
			//       = c0*x^6 + c1*x^5 + c2*x^4 + c3*x^3 + c4*x^2 + c5*x + v_i mod g(x)
			//       = c0*(x^6 mod g(x)) + c1*x^5 + c2*x^4 + c3*x^3 + c4*x^2 + c5*x + v_i
			// If we call (x^6 mod g(x)) = k(x), this can be written as
			// c'(x) = (c1*x^5 + c2*x^4 + c3*x^3 + c4*x^2 + c5*x + v_i) + c0*k(x)

			// First, determine the value of c0:
			uint8_t c0 = c >> 25;

			// Then compute c1*x^5 + c2*x^4 + c3*x^3 + c4*x^2 + c5*x + v_i:
			c = ((c & 0x1ffffff) << 5) ^ v_i;

			// Finally, for each set bit n in c0, conditionally add {2^n}k(x):
			if (c0 & 1)  c ^= 0x3b6a57b2; //     k(x) = {29}x^5 + {22}x^4 + {20}x^3 + {21}x^2 + {29}x + {18}
			if (c0 & 2)  c ^= 0x26508e6d; //  {2}k(x) = {19}x^5 +  {5}x^4 +     x^3 +  {3}x^2 + {19}x + {13}
			if (c0 & 4)  c ^= 0x1ea119fa; //  {4}k(x) = {15}x^5 + {10}x^4 +  {2}x^3 +  {6}x^2 + {15}x + {26}
			if (c0 & 8)  c ^= 0x3d4233dd; //  {8}k(x) = {30}x^5 + {20}x^4 +  {4}x^3 + {12}x^2 + {30}x + {29}
			if (c0 & 16) c ^= 0x2a1462b3; // {16}k(x) = {21}x^5 +     x^4 +  {8}x^3 + {24}x^2 + {21}x + {19}
		}
		return c;
	}

	/** Expand a HRP for use in checksum computation. */
	[[nodiscard]] static std::vector<uint8_t> expand_hrp(const std::string& hrp)
	{
		std::vector<uint8_t> ret;
		ret.reserve(hrp.size() + 90);
		ret.resize(hrp.size() * 2 + 1);
		for (size_t i = 0; i < hrp.size(); ++i)
		{
			unsigned char c = hrp[i];
			ret[i] = c >> 5;
			ret[i + hrp.size() + 1] = c & 0x1f;
		}
		ret[hrp.size()] = 0;
		return ret;
	}

	[[nodiscard]] static std::vector<uint8_t> create_checksum(const std::string& hrp, const std::vector<uint8_t>& values)
	{
		std::vector<uint8_t> enc = cat(expand_hrp(hrp), values);
		enc.resize(enc.size() + 6);
		uint32_t mod = polymod(enc) ^ 1;
		std::vector<uint8_t> ret;
		ret.resize(6);
		for (size_t i = 0; i < 6; ++i) {
			// Convert the 5-bit groups in mod to checksum values.
			ret[i] = (mod >> (5 * (5 - i))) & 31;
		}
		return ret;
	}

	std::string bech32::encode(const std::string& hrp, const std::vector<uint8_t>& values)
	{
		std::vector<uint8_t> checksum = create_checksum(hrp, values);
		std::vector<uint8_t> combined = cat(values, checksum);
		std::string ret = hrp + '1';
		ret.reserve(ret.size() + combined.size());
		for (const auto c : combined)
		{
			ret += CHARSET[c];
		}
		return ret;
	}
}
