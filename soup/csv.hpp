#pragma once

#include <string>
#include <vector>

namespace soup
{
	struct csv
	{
		[[nodiscard]] static std::vector<std::string> parseLine(const std::string& line);
	};
}
