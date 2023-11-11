#pragma once

#include <string>

#include "base.hpp"

namespace soup
{
	struct CountryNamesEntry
	{
		const char* country_code; // alpha-2
		const char* language_code;
#if SOUP_CPP20
		const char8_t* value;
#else
		const char* value;
#endif
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
						return (const char*)i->value;
					}
				} while (++i, i->country_code == cc);
				break;
			}
		}
		return nullptr;
	}

	[[nodiscard]] inline const char* getCountryCode(const std::string& country_name, const std::string& language_code = "EN")
	{
		CountryNamesEntry* i = &country_names[0];

		// Seek ahead to language code
		for (;; ++i)
		{
			if (i->language_code == language_code)
			{
				break;
			}
			if (i == &country_names[COUNTRYNAMES_LANGUAGES])
			{
				// Exhausted all languages, abort lookup.
				return nullptr;
			}
		}

		// Find country
		for (; i < &country_names[COUNTRYNAMES_COUNTRIES * COUNTRYNAMES_LANGUAGES]; i += COUNTRYNAMES_LANGUAGES)
		{
			if (country_name == (const char*)i->value)
			{
				return i->country_code;
			}
		}

		return nullptr;
	}
}
