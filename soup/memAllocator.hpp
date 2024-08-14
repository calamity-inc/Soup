#pragma once

#include "base.hpp"

#include <cstddef> // size_t

NAMESPACE_SOUP
{
	class memAllocator
	{
	protected:
		using allocate_impl_t = void*(*)(memAllocator*, size_t) /* SOUP_EXCAL */;
		using deallocate_impl_t = void(*)(memAllocator*, void*) /* noexcept */;

		const allocate_impl_t allocate_impl;
		const deallocate_impl_t deallocate_impl;

		explicit memAllocator(allocate_impl_t allocate_impl, deallocate_impl_t deallocate_impl) noexcept
			: allocate_impl(allocate_impl), deallocate_impl(deallocate_impl)
		{
		}

	public:
		[[nodiscard]] void* allocate(size_t size) SOUP_EXCAL
		{
			return allocate_impl(this, size);
		}

		void deallocate(void* addr) noexcept
		{
			return deallocate_impl(this, addr);
		}
	};
}
