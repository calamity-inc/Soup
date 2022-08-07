#pragma once

#include "ShortString.hpp"

namespace soup
{
	struct netIntelLocationData
	{
		ShortString<3> country_code;
		const char* state;
		const char* city;
	};
}
