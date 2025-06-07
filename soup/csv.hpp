#pragma once

#include <string>
#include <vector>

#include "base.hpp"

NAMESPACE_SOUP
{
	struct csv
	{
		static void parseLine(std::vector<std::string>& res, const std::string& line);
	};
}
