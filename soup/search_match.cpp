#include "search_match.hpp"

#include "string.hpp"

namespace soup
{
	bool search_match(const std::string& query, std::string item)
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
}
