#pragma once

#include "AllocRaiiLocalBase.hpp"

#include "base.hpp"

#if SOUP_WINDOWS
#include <Windows.h>
#else
#include <sys/mman.h>
#endif

#include "memProtFlags.hpp"

namespace soup
{
	struct AllocRaiiVirtual : public AllocRaiiLocalBase
	{
		size_t size;

		AllocRaiiVirtual(size_t size, int prot_flags = (MEM_PROT_READ | MEM_PROT_WRITE | MEM_PROT_EXEC))
			: AllocRaiiLocalBase(
#if SOUP_WINDOWS
				VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, memProtFlagsToProtect(prot_flags))
#else
				mmap(nullptr, size, prot_flags, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)
#endif
			), size(size)
		{
		}

		~AllocRaiiVirtual()
		{
#if SOUP_WINDOWS
			VirtualFree(addr, size, MEM_DECOMMIT);
#else
			munmap(addr, size);
#endif
		}
	};
}
