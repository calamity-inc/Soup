#pragma once

#include <string>

#include "ShortString.hpp"

NAMESPACE_SOUP
{
	struct netIntelLocationData
	{
		ShortString<2> country_code;
		const char* state;
		const char* city;
	};

	struct netIntelLocationDataSelfContained
	{
		ShortString<2> country_code;
		std::string state;
		std::string city;
	};
}
