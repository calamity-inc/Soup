#include "StringMatch.hpp"

#include "string.hpp"
#include "StringBuilder.hpp"

namespace soup
{
	bool StringMatch::search(const std::string& query, std::string item)
	{
		string::lower(item);

		auto words = string::explode(query, ' ');
		for (const auto& word : words)
		{
			if (item.find(word) == std::string::npos)
			{
				return false;
			}
		}
		return true;
	}

	bool StringMatch::wildcard(const std::string& query, const std::string& item, size_t min_chars_per_wildcard)
	{
		if (item.empty())
		{
			return query.empty();
		}

		size_t ii = 0;
		auto qi = query.begin();

		StringBuilder sb;
		sb.beginCopy(query, qi);
		while (qi != query.end())
		{
			if (*qi == '*')
			{
				sb.endCopy(query, qi);
				auto res = item.find(sb, ii);
				if (res == std::string::npos)
				{
					return false;
				}
				ii = (res + sb.size());
				ii += min_chars_per_wildcard;
				if (ii > item.size())
				{
					return false;
				}
				++qi;
				sb.clear();
				sb.beginCopy(query, qi);
			}
			else
			{
				++qi;
			}
		}
		sb.endCopy(query, qi);
		if (item.length() < sb.length() || item.length() - sb.length() < ii || item.substr(item.length() - sb.length()) != sb)
		{
			return false;
		}
		return true;
	}
}
