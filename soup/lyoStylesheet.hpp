#pragma once

#include <string>
#include <vector>

#include "lyoStyle.hpp"

namespace soup
{
	struct lyoRule
	{
		std::string selector;
		lyoStyle style;
	};

	struct lyoStylesheet
	{
		std::string name;
		std::vector<lyoRule> rules;
	};
}
