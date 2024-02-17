#pragma once

#include "base.hpp"

#include <cstdint>
#include <string>

namespace soup
{
	struct sha512
	{
		[[nodiscard]] static std::string hash(const void* data, size_t len) SOUP_EXCAL;
		[[nodiscard]] static std::string hash(const std::string& str) SOUP_EXCAL;

		static void processBlock(uint64_t block[16], uint64_t h[8]);
	};
}
