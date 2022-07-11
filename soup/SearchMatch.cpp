#include "SearchMatch.hpp"

#include "string.hpp"

namespace soup
{
	using Strategy = SearchMatch::Strategy;
	using search_match_t = SearchMatch::search_match_t;

	static bool strategy_impl_all_letters_in_order_not_necessarily_consecutive(const std::string& query, const std::string& item)
	{
		auto i = query.begin();
		for (const auto& c : string::lower(std::string(item)))
		{
			if (c == *i)
			{
				++i;
			}
		}
		return i == query.end();
	}

	static bool strategy_impl_substring(const std::string& query, const std::string& item)
	{
		return string::lower(std::string(item)).find(query) != std::string::npos;
	}

	bool SearchMatch::run(const std::string& query, const std::string& item)
	{
		return getFunctionForQuery(query)(query, item);
	}

	Strategy SearchMatch::getStrategyForQuery(const std::string& query)
	{
		if (query.find(' ') != std::string::npos)
		{
			return ALL_LETTERS_IN_ORDER_NOT_NECESSARILY_CONSECUTIVE;
		}
		return SUBSTRING;
	}

	search_match_t SearchMatch::getFunctionForStrategy(Strategy s)
	{
		if (s == ALL_LETTERS_IN_ORDER_NOT_NECESSARILY_CONSECUTIVE)
		{
			return &strategy_impl_all_letters_in_order_not_necessarily_consecutive;
		}
		return &strategy_impl_substring;
	}

	search_match_t SearchMatch::getFunctionForQuery(const std::string& query)
	{
		return getFunctionForStrategy(getStrategyForQuery(query));
	}

	bool SearchMatch::run(Strategy s, const std::string& query, const std::string& item)
	{
		return getFunctionForStrategy(s)(query, item);
	}
}
