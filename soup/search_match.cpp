#include "search_match.hpp"

#include "string.hpp"

namespace soup
{
	bool search_match(const std::string& query, const std::string& item)
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
}
