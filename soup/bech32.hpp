#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace soup
{
	// Adapted from https://github.com/sipa/bech32

	struct bech32
	{
		[[nodiscard]] static std::string encode(const std::string& hrp, const std::vector<uint8_t>& values);
	};
}
