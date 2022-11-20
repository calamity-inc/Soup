#include "version_compare.hpp"

#include "string.hpp"

namespace soup
{
	strong_ordering version_compare(const std::string& in_a, const std::string& in_b)
	{
		std::vector<unsigned long> a{};
		std::vector<unsigned long> b{};

		for (const auto& s : string::explode(in_a, '.'))
		{
			a.emplace_back(std::stoul(s));
		}
		for (const auto& s : string::explode(in_b, '.'))
		{
			b.emplace_back(std::stoul(s));
		}

		if (a.size() != b.size())
		{
			if (a.size() > b.size())
			{
				b.insert(b.end(), a.size() - b.size(), 0);
			}
			else
			{
				a.insert(a.end(), b.size() - a.size(), 0);
			}
		}

		for (size_t i = 0; i != a.size(); ++i)
		{
			auto c = SOUP_SPACESHIP(a[i], b[i]);
			if (c != 0)
			{
				return c;
			}
		}

		return strong_ordering::equal;
	}
}
