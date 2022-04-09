#pragma once

#include <algorithm>
#include <utility>
#include <vector>

#include "unique_ptr.hpp"

namespace soup
{
	template <typename Key, typename Value>
	class derivable_map
	{
	protected:
		std::vector<std::pair<unique_ptr<Key>, Value>> data{};
		bool unsorted = false;

		using DataIt = typename std::vector<std::pair<unique_ptr<Key>, Value>>::iterator;
		using DataCIt = typename std::vector<std::pair<unique_ptr<Key>, Value>>::const_iterator;

	public:
		void ensureSorted()
		{
			if (unsorted)
			{
				unsorted = false;
				std::sort(data.begin(), data.end(), [](const std::pair<unique_ptr<Key>, Value>& a, const std::pair<unique_ptr<Key>, Value>& b)
				{
					return *a.first < *b.first;
				});
			}
		}

		void emplace(unique_ptr<Key>&& k, Value&& v)
		{
			data.emplace_back(std::move(k), std::move(v));
			unsorted = true;
		}

		std::pair<unique_ptr<Key>, Value>* find(const Key& k)
		{
			ensureSorted();
			return find(k, data.begin(), data.end());
		}

	protected:
		[[nodiscard]] std::pair<unique_ptr<Key>, Value>* find(const Key& k, DataIt first, DataIt last)
		{
			while (true)
			{
				auto size = std::distance(first, last);
				DataIt pivot = first + (size / 2);
				auto cmp = (k <=> *pivot->first);
				if (cmp == 0)
				{
					return &*pivot;
				}
				if (size == 1)
				{
					return nullptr;
				}
				if (cmp < 0)
				{
					last = pivot;
				}
				else
				{
					first = pivot;
				}
			}
		}

	public:
		[[nodiscard]] bool empty() const noexcept
		{
			return data.empty();
		}

		[[nodiscard]] bool contains(const Key& k)
		{
			return find(k) != nullptr;
		}

		[[nodiscard]] Value& at(const Key& k)
		{
			return find(k)->second;
		}

		DataIt begin()
		{
			ensureSorted();
			return data.begin();
		}

		DataIt end() noexcept
		{
			return data.end();
		}

		DataCIt begin() const noexcept
		{
			return data.cbegin();
		}

		DataCIt end() const noexcept
		{
			return data.cend();
		}
	};
}
