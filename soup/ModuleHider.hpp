#pragma once

#include "base.hpp"

#if SOUP_WINDOWS && SOUP_BITS == 64
#include <Windows.h>

namespace soup
{
	class ModuleHider
	{
	private:
		void* hidden_entry = nullptr;
		void* hidden_pNext = nullptr;

	public:
		void enable(HMODULE mod);
		void enable(void* base_addr);

		void disable();
	};
}
#endif
