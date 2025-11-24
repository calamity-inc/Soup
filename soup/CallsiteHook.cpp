#include "CallsiteHook.hpp"
#if SOUP_X86

#include <cstring> // memset

#include "memGuard.hpp"
#include "Pointer.hpp"

NAMESPACE_SOUP
{
	void CallsiteHook::create()
	{
		original = Pointer(target).followJumps().add(1).rip().as<void*>();

#if SOUP_BITS == 64
		memGuard::setAllowedAccess(code_cave, sizeof(longjump_trampoline_r10), memGuard::ACC_RWX);
		writeLongjumpTrampolineR10(code_cave, detour);
#else
		memGuard::setAllowedAccess(code_cave, sizeof(longjump_trampoline_noreg), memGuard::ACC_RWX);
		writeLongjumpTrampolineNoreg(code_cave, detour);
#endif
	}

	void CallsiteHook::destroy()
	{
#if SOUP_BITS == 64
		memset(code_cave, 0xCC, sizeof(longjump_trampoline_r10));
#else
		memset(code_cave, 0xCC, sizeof(longjump_trampoline_noreg));
#endif
	}

	void CallsiteHook::enable()
	{
		void* addr = getEffectiveTarget();
		memGuard::setAllowedAccess(addr, sizeof(call_trampoline), memGuard::ACC_RWX);
		writeCallTrampoline(addr, code_cave);
	}

	void CallsiteHook::disable()
	{
		writeCallTrampoline(getEffectiveTarget(), original);
	}
}

#endif
