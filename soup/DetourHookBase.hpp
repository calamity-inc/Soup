#pragma once

#include "HookBase.hpp"
#if SOUP_X86

#include <cstddef> // size_t

NAMESPACE_SOUP
{
	struct DetourHookBase : public HookBase
	{
		void* original = nullptr;

		void createOriginal(size_t trampoline_bytes);
		void destroyOriginal() noexcept;
	};
}

#endif
