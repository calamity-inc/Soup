#pragma once

#include "CryptoHashAlgo.hpp"

namespace soup
{
	struct sha256 : public CryptoHashAlgo<sha256>
	{
		static constexpr unsigned char OID[] = { 0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20 };
		static constexpr auto DIGEST_BYTES = 32u;
		static constexpr auto BLOCK_BYTES = 64u;

		[[nodiscard]] static std::string hash(const std::string& s);
		[[nodiscard]] static std::string hash(std::string&& s);
		[[nodiscard]] static std::string hash(std::istream& is);
	};
}
