#include "search_match.hpp"

namespace soup
{
	bool search_match(const std::string& query, const std::string& item)
	{
		auto i = query.begin();
		for (const auto& c : item)
		{
			if (c == *i)
			{
				++i;
			}
		}
		return i == query.end();
	}
}
