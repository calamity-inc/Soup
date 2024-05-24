#pragma once

#include <windows.h>

#include "Pointer.hpp"

NAMESPACE_SOUP
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
