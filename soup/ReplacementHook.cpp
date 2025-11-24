#include "ReplacementHook.hpp"
#if SOUP_X86 && SOUP_BITS == 64

#include "memGuard.hpp"

NAMESPACE_SOUP
{
	void ReplacementHook::enable()
	{
		void* addr = getEffectiveTarget();
		memGuard::setAllowedAccess(addr, sizeof(longjump_trampoline_r10), memGuard::ACC_RWX);
		writeLongjumpTrampolineR10(addr, detour);
	}
}

#endif
