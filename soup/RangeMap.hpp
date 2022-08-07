#pragma once

#include <vector>

namespace soup
{
	template <typename K, typename V>
	class RangeMap
	{
	private:
		struct Entry
		{
			K lower;
			K upper;
			V data;
		};

		std::vector<Entry> data;

	public:
		V& emplace(K begin, K end, V val)
		{
			return data.emplace_back(Entry{ std::move(begin), std::move(end), std::move(val) }).data;
		}

		[[nodiscard]] V* find(const K& k)
		{
			if (data.empty())
			{
				return nullptr;
			}
			auto first = data.begin();
			auto last = data.end();
			while (true)
			{
				auto size = std::distance(first, last);
				auto pivot = first + (size / 2);
				if (pivot->lower <= k && k <= pivot->upper)
				{
					return &pivot->data;
				}
				if (size == 1)
				{
					return nullptr;
				}
				if (pivot->lower < k)
				{
					first = pivot;
				}
				else
				{
					last = pivot;
				}
			}
		}
	};
}
