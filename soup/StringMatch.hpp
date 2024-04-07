#pragma once

#include <string>

#include "base.hpp"

NAMESPACE_SOUP
{
	struct StringMatch
	{
		[[nodiscard]] static bool search(const std::string& query, std::string item); // assumes that query is lowercase
		[[nodiscard]] static bool wildcard(const std::string& query, const std::string& item, size_t min_chars_per_wildcard = 0); // case-sensitive
	};
}
