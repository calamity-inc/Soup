#pragma once

#include "base.hpp"

#include <string>

namespace soup
{
	struct sha384
	{
		[[nodiscard]] static std::string hash(const void* data, size_t len) SOUP_EXCAL;
		[[nodiscard]] static std::string hash(const std::string& str) SOUP_EXCAL;
	};
}
