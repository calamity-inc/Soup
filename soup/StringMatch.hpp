#pragma once

#include <string>

namespace soup
{
	struct StringMatch
	{
		[[nodiscard]] static bool search(const std::string& query, std::string item); // assumes that query is lowercase
	};
}
