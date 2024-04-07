#include "SegWitAddress.hpp"

#include <vector>

#include "bech32.hpp"
#include "bitutil.hpp"
#include "ripemd160.hpp"
#include "sha256.hpp"

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
	std::string SegWitAddress::encode() const
	{
		const char* hrp = (network == TESTNET ? "tb" : "bc");
		auto witprog = ripemd160(sha256::hash(compressed_pub_key));

		auto chunks = bitutil::msb_first<std::vector<uint8_t>, 8, 5>(witprog);
		chunks.insert(chunks.begin(), 1, version);
		return bech32::encode(hrp, chunks);
	}
}
