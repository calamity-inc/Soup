#pragma once

#include <string>

namespace soup
{
	struct base58
	{
		[[nodiscard]] static std::string decode(const std::string& in);
	};
}
