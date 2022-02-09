#pragma once

#include "crypto_hash_algo.hpp"

namespace soup
{
	struct sha1 : public crypto_hash_algo<sha1>
	{
		static constexpr char ID[] = { 0x30, 0x21, 0x30, 0x09, 0x06, 0x05, 0x2B, 0x0E, 0x03, 0x02, 0x1A, 0x05, 0x00, 0x04, 0x14 };
		static constexpr auto DIGEST_BYTES = 20u;

		[[nodiscard]] static std::string hash(const std::string& s);
		[[nodiscard]] static std::string hash(std::istream& is);
	};
}
