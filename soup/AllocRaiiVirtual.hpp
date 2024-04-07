#pragma once

#include "AllocRaiiLocalBase.hpp"

#include "memProtFlags.hpp"
#include "os.hpp"

NAMESPACE_SOUP
{
	struct AllocRaiiVirtual : public AllocRaiiLocalBase
	{
		size_t size;

		AllocRaiiVirtual(size_t size, int prot = (MEM_PROT_READ | MEM_PROT_WRITE | MEM_PROT_EXEC))
			: AllocRaiiLocalBase(os::virtualAlloc(size, prot)), size(size)
		{
		}

		~AllocRaiiVirtual()
		{
			os::virtualFree(addr, size);
		}
	};
}
