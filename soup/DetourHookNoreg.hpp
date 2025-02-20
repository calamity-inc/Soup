#pragma once

#include "DetourHookBase.hpp"

NAMESPACE_SOUP
{
	struct DetourHookNoreg : public DetourHookBase
	{
		[[nodiscard]] bool isCreated() const noexcept { return original != nullptr; }
		void create() { return createOriginal(sizeof(longjump_trampoline_noreg)); }
		void destroy() noexcept { return destroyOriginal(); }

		void enable();
		void disable();
	};
}
