#pragma once

#include "filesystem.hpp"
#if !SOUP_WASM || SOUP_EMSCRIPTEN

NAMESPACE_SOUP
{
	template <typename K, typename V>
	struct MemoryMappedMap
	{
		struct Entry
		{
			const K key;
			const V value;
		};

		const Entry* begin;
		const Entry* end;

		constexpr MemoryMappedMap() noexcept
			: begin(nullptr), end(nullptr)
		{
		}

		MemoryMappedMap(const void* data, size_t size) noexcept
			: begin((const Entry*)data), end((const Entry*)((const char*)data + size))
		{
		}

		void init(const void* data, size_t size) noexcept
		{
			reset();
			begin = (const Entry*)data;
			end = (const Entry*)((const char*)data + size);
		}

		~MemoryMappedMap() noexcept
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
				if (pivot->key == k)
				{
					return &pivot->value;
				}
				if (size == 1)
				{
					return nullptr;
				}
				if (pivot->key < k)
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

#endif
