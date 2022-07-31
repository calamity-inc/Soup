#pragma once

#include "AllocRaiiLocalBase.hpp"

#include <Windows.h>

namespace soup
{
	struct AllocRaiiVirtual : public AllocRaiiLocalBase
	{
		size_t size;

		AllocRaiiVirtual(size_t size)
			: AllocRaiiLocalBase(VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE)), size(size)
		{
		}

		~AllocRaiiVirtual()
		{
			VirtualFree(addr, size, MEM_DECOMMIT);
		}
	};
}
