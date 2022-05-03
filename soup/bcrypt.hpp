#pragma once

#include <string>

namespace soup
{
	struct bcrypt
	{
		[[nodiscard]] static std::string hash(const std::string& password, unsigned rounds = 10);
		[[nodiscard]] static bool verify(const std::string& password, const std::string& hash);
	};
}
