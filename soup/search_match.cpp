#include "search_match.hpp"

#include "string.hpp"

namespace soup
{
	bool search_match(const std::string& query, const std::string& item)
	{
		auto i = query.begin();
		size_t consecutive = 0;
		size_t longest_consecutive = 0;
		for (const auto& c : string::lower(std::string(item)))
		{
			if (c == *i)
			{
				++i;
				++consecutive;
			}
			else
			{
				if (consecutive != 0)
				{
					if (longest_consecutive < consecutive)
					{
						longest_consecutive = consecutive;
					}
					consecutive = 0;
				}
			}
		}
		if (longest_consecutive < consecutive)
		{
			longest_consecutive = consecutive;
		}
		size_t consecutive_treshold = query.length() > 2 ? 2 : 1;
		return i == query.end()
			&& longest_consecutive > consecutive_treshold
			;
	}
}
