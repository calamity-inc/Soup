#pragma once

#include <string>
#include <vector>

#include "base.hpp"

NAMESPACE_SOUP
{
	struct csv
	{
		[[nodiscard]] static std::vector<std::string> parseLine(const std::string& line);
	};
}
