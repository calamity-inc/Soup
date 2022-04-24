#pragma once

#include <Windows.h>

#include "Pointer.hpp"

namespace soup
{
	struct AllocRaiiRemote
	{
		HANDLE h;
		Pointer p;
		size_t size;

		~AllocRaiiRemote()
		{
			VirtualFreeEx(h, p.as<void*>(), size, MEM_DECOMMIT);
		}
	};
}
