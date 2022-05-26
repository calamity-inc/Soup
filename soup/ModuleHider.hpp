#pragma once

#include "base.hpp"

#if SOUP_WINDOWS && SOUP_BITS == 64
#include <Windows.h>

namespace soup
{
	class ModuleHider
	{
	private:
		void* ldr_data_table_entry = nullptr;

	public:
		void enable(HMODULE mod);
		void enable(void* base_addr);

		void disable();
	};
}
#endif
