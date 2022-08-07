#pragma once

#include "ShortString.hpp"

namespace soup
{
	struct netIntelLocationData
	{
		ShortString<3> country_code;
		std::string state;
		std::string city;
	};
}
