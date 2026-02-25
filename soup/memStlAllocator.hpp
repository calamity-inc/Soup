#pragma once

#include "memAllocator.hpp"

NAMESPACE_SOUP
{
	/* Example usage:
	memCAllocator alloc;
	std::vector<int, memStlAllocator<int>> vec({ 1, 2, 3 }, memStlAllocator<int>(alloc));
	std::cout << vec[0]; // 1
	*/
	template <typename T>
	struct memStlAllocator
	{
		using value_type = T;

		memAllocator& underlying;

		memStlAllocator(memAllocator& underlying)
			: underlying(underlying)
		{
		}

		[[nodiscard]] T* allocate(size_t count) const SOUP_EXCAL
		{
			return (T*)underlying.allocate(count * sizeof(T));
		}

		void deallocate(void* addr, size_t count) const noexcept
		{
			return underlying.deallocate(addr);
		}
	};
}
