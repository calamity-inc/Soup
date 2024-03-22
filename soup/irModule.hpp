#pragma once

#include <string>
#include <vector>

#include "irFunction.hpp"

namespace soup
{
	struct irModule
	{
		std::string data{};
		std::vector<irFunction> func_exports{};
	};
}
