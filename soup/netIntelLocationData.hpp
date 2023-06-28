#pragma once

#include "ShortString.hpp"

namespace soup
{
	struct netIntelLocationData
	{
		ShortString<2> country_code;
		const char* state;
		const char* city;
	};
}
