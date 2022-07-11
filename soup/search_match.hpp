#pragma once

#include <string>

namespace soup
{
	[[nodiscard]] bool search_match(const std::string& query, const std::string& item); // assumes both strings are lowercase
}
