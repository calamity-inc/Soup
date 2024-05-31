#pragma once

#include "AllocRaiiLocalBase.hpp"

#include "memGuard.hpp"

NAMESPACE_SOUP
{
	struct AllocRaiiVirtual : public AllocRaiiLocalBase
	{
		size_t size;

		AllocRaiiVirtual(size_t size, int allowed_access = memGuard::ACC_RWX)
			: AllocRaiiLocalBase(memGuard::alloc(size, allowed_access)), size(size)
		{
		}

		~AllocRaiiVirtual()
		{
			memGuard::free(addr, size);
		}
	};
}
