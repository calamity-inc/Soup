#pragma once

#include "DetourHookBase.hpp"

#if SOUP_X86 && SOUP_BITS == 64

NAMESPACE_SOUP
{
	// Uses r10 to store the jump target
	struct DetourHook : public DetourHookBase
	{
		[[nodiscard]] bool isCreated() const noexcept { return original != nullptr; }
		void create() { return createOriginal(sizeof(longjump_trampoline_r10)); }
		void destroy() noexcept { return destroyOriginal(); }

		void enable();
		void disable();
	};
}

#endif
