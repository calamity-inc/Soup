#include "bitutil.hpp"

namespace soup
{
	std::vector<bool> bitutil::interleave(const std::vector<std::vector<bool>>& data)
	{
		const auto inner_size = data.at(0).size();
		const auto outer_size = data.size();
		std::vector<bool> out(inner_size * outer_size, false);
		size_t outer_i = 0;
		size_t inner_i = 0;
		for (size_t i = 0;; ++i)
		{
			out.at(i) = data.at(outer_i).at(inner_i);
			if (++outer_i == outer_size)
			{
				outer_i = 0;
				if (++inner_i == inner_size)
				{
					break;
				}
			}
		}
		return out;
	}
}
