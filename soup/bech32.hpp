#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "base.hpp"

NAMESPACE_SOUP
{
	struct bech32
	{
		[[nodiscard]] static std::string encode(const std::string& hrp, const std::vector<uint8_t>& values);
	};
}
