#pragma once

#include "base.hpp"

#if SOUP_WINDOWS && SOUP_BITS == 64
#include <Windows.h>

namespace soup
{
	class ModuleHider
	{
	private:
		uintptr_t InLoadOrderModuleList_links = 0;
		uintptr_t* InLoadOrderModuleList_pNext;
		uintptr_t InMemoryOrderModuleList_links = 0;
		uintptr_t* InMemoryOrderModuleList_pNext;
		//uintptr_t InInitializationOrderModuleList_links = 0;
		//uintptr_t* InInitializationOrderModuleList_pNext;

	public:
		void enable(HMODULE mod);
		void enable(void* base_addr);

		void disable();
	};
}
#endif
