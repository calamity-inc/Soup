#pragma once

#include <string>
#include <vector>

#include "lyoStyle.hpp"

NAMESPACE_SOUP
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

		void addRulesFromCss(const std::string& css);
	};
}
