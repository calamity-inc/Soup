#pragma once

#include "filesystem.hpp"
#if !SOUP_WASM || SOUP_EMSCRIPTEN

NAMESPACE_SOUP
{
	template <typename K, typename V>
	struct MemoryMappedRangeMap
	{
		struct Entry
		{
			const K lower;
			const K upper;
			const V data;
		};

		const Entry* begin;
		const Entry* end;

		constexpr MemoryMappedRangeMap() noexcept
			: begin(nullptr), end(nullptr)
		{
		}

		MemoryMappedRangeMap(const void* data, size_t size) noexcept
			: begin((const Entry*)data), end((const Entry*)((const char*)data + size))
		{
		}

		void init(const void* data, size_t size) noexcept
		{
			reset();
			begin = (const Entry*)data;
			end = (const Entry*)((const char*)data + size);
		}

		~MemoryMappedRangeMap() noexcept
		{
			const size_t size = (const char*)end - (const char*)begin;
			filesystem::destroyFileMapping((const void*)begin, size);
		}

		void reset() noexcept
		{
			if (begin)
			{
				const size_t size = (const char*)end - (const char*)begin;
				filesystem::destroyFileMapping((const void*)begin, size);
				begin = nullptr;
			}
		}

		[[nodiscard]] const V* find(const K& k) const
		{
			if (begin == end)
			{
				return nullptr;
			}
			auto first = begin;
			auto last = end;
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
	static_assert(sizeof(MemoryMappedRangeMap<uint32_t, uint32_t>::Entry) == 3 * 4);
}

#endif
