#pragma once

#include "HookBase.hpp"
#if SOUP_X86 && SOUP_BITS == 64

NAMESPACE_SOUP
{
	struct ReplacementHook : public HookBase
	{
		void enable();
	};
}

#endif
