#pragma once

#include <Windows.h>

#include "pointer.hpp"

namespace soup
{
	struct alloc_raii_remote
	{
		HANDLE h;
		pointer p;
		size_t size;

		~alloc_raii_remote()
		{
			VirtualFreeEx(h, p.as<void*>(), size, MEM_DECOMMIT);
		}
	};
}
