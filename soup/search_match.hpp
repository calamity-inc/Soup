#pragma once

#include <string>

namespace soup
{
	[[nodiscard]] bool search_match(const std::string& query, std::string item); // assumes that query is lowercase
}
