#pragma once

#include <string>

#include "base.hpp"

NAMESPACE_SOUP
{
	struct base58
	{
		[[nodiscard]] static std::string decode(const std::string& in);
	};
}
