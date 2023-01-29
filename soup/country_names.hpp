#pragma once

#include <string>

namespace soup
{
	struct CountryNamesEntry
	{
		const char* country_code; // alpha-2
		const char* language_code;
		const char* value;
	};

#define COUNTRYNAMES_COUNTRIES 249
#define COUNTRYNAMES_LANGUAGES 81

	extern CountryNamesEntry country_names[COUNTRYNAMES_COUNTRIES * COUNTRYNAMES_LANGUAGES];

	[[nodiscard]] inline const char* getCountryName(const std::string& country_code, const std::string& language_code = "EN")
	{
		for (CountryNamesEntry* i = &country_names[0]; i != &country_names[COUNTRYNAMES_COUNTRIES * COUNTRYNAMES_LANGUAGES]; i += COUNTRYNAMES_LANGUAGES)
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
