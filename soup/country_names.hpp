#pragma once

#include <string>

#include <iostream>

namespace soup
{
	struct CountryNamesEntry
	{
		const char* country_code; // alpha-2
		const char* language_code;
		const char* value;
	};

	extern CountryNamesEntry country_names[20169]; // = 249 countries * 81 languages

	[[nodiscard]] inline const char* getCountryName(const std::string& country_code, const std::string& language_code = "EN")
	{
		for (CountryNamesEntry* i = &country_names[0]; i != &country_names[20169]; i += 81) // for each country, there is 81 entries, one for each supported language
		{
			if (country_code == i->country_code)
			{
				const char* cc = i->country_code;
				do
				{
					if (language_code == i->language_code)
					{
						return i->value;
					}
				} while (++i, i->country_code == cc);
				break;
			}
		}
		return nullptr;
	}
}
