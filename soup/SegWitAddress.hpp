#pragma once

#include <cstdint>
#include <string>

namespace soup
{
	struct SegWitAddress
	{
		enum Network : uint8_t
		{
			MAINNET,
			TESTNET,
		};

		uint8_t version = 0;
		Network network = MAINNET;
		std::string compressed_pub_key; // 0x02 or 0x03 followed by 32 byte X coordinate

		[[nodiscard]] std::string encode() const;
	};
}
