#pragma once

#include <string>

namespace soup
{
	// Query is always assumed to be lowercase. Item can be whatever.
	struct SearchMatch
	{
		enum Strategy : uint8_t
		{
			SUBSTRING = 0,
			ALL_LETTERS_IN_ORDER_NOT_NECESSARILY_CONSECUTIVE,
		};

		using search_match_t = bool(*)(const std::string& query, const std::string& item);

		[[nodiscard]] static bool run(const std::string& query, const std::string& item);

		[[nodiscard]] static Strategy getStrategyForQuery(const std::string& query);
		[[nodiscard]] static search_match_t getFunctionForStrategy(Strategy s);
		[[nodiscard]] static search_match_t getFunctionForQuery(const std::string& query);
		[[nodiscard]] static bool run(Strategy s, const std::string& query, const std::string& item);
	};
}
