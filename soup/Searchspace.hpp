#pragma once

#include "base.hpp"

NAMESPACE_SOUP
{
	struct Searchspace
	{
		struct Range
		{
			size_t begin;
			size_t end;
		};

		// begin = 0
		size_t end;

		[[nodiscard]] std::vector<Range> split(size_t parts) const
		{
			if (parts == 1)
			{
				return { Range{ 0, end } };
			}
			std::vector<Range> res{};
			res.reserve(parts);
			size_t part_size = (end / parts);
			size_t coverage = (part_size * parts);
			--parts;
			for (size_t i = 0; i != parts; ++i)
			{
				res.emplace_back(Range{ i * part_size, part_size });
			}
			res.emplace_back(Range{ parts * part_size, part_size + (end - coverage) });
			return res;
		}
	};
}
