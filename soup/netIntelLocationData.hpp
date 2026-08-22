#pragma once

#include <string>

#include "ShortString.hpp"

NAMESPACE_SOUP
{
	struct netIntelLocationData
	{
		ShortString<2> country_code;
		const char* state; // will never be nullptr. "" if not known.
		const char* city; // will never be nullptr. "" if not known.
	};
}
