#include "DetourHookNoreg.hpp"
#if SOUP_X86

#include <cstring> // memcpy

#include "memGuard.hpp"

NAMESPACE_SOUP
{
	void DetourHookNoreg::enable()
	{
		void* addr = getEffectiveTarget();
		memGuard::setAllowedAccess(addr, sizeof(longjump_trampoline_noreg), memGuard::ACC_RWX);
		writeLongjumpTrampolineNoreg(addr, detour);
	}

	void DetourHookNoreg::disable()
	{
		memcpy(getEffectiveTarget(), original, sizeof(longjump_trampoline_noreg));
	}
}

#endif
