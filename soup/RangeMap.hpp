#pragma once

#include <algorithm>
#include <vector>

#include "base.hpp"

NAMESPACE_SOUP
{
	template <typename K, typename V>
	struct RangeMap
	{
		struct Entry
		{
			K lower;
			K upper;
			V data;
		};

		std::vector<Entry> data;

		void reserve(size_t i)
		{
			data.reserve(i);
		}

		V& emplace(K begin, K end, V val)
		{
			return data.emplace_back(Entry{ std::move(begin), std::move(end), std::move(val) }).data;
		}

		void sort()
		{
			std::sort(data.begin(), data.end(), [](const Entry& a, const Entry& b)
			{
				return a.lower < b.lower;
			});
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

		[[nodiscard]] const V* find(const K& k) const
		{
			return const_cast<RangeMap<K, V>*>(this)->find(k);
		}

		void clear() noexcept
		{
			data.clear();
			data.shrink_to_fit();
		}
	};
}
