#pragma once

#include "base.hpp"

#include <string>

#include "joaat.hpp"
#include "loc.hpp"

NAMESPACE_SOUP
{
	[[nodiscard]] inline std::string getCountryName(const char* country_code, const std::string& language_code = "EN")
	{
		const auto dict = loc::getDict(language_code);
		if (auto e = dict.find(joaat::concat(joaat::compileTimeHash("COUNTRY_NAME_"), country_code)); e != dict.end())
		{
			return e->second;
		}
		return {};
	}
}
