#include "SegWitAddress.hpp"

#include <vector>

#include "bech32.hpp"
#include "bitutil.hpp"
#include "ripemd160.hpp"
#include "sha256.hpp"

namespace soup
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
