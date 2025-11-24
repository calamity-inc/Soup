#include "CompactDetourHook.hpp"
#if SOUP_X86

#include <cstring> // memset, memcpy

#include "memGuard.hpp"

NAMESPACE_SOUP
{
	void CompactDetourHook::create()
	{
#if SOUP_BITS == 64
		memGuard::setAllowedAccess(code_cave, sizeof(longjump_trampoline_r10), memGuard::ACC_RWX);
		writeLongjumpTrampolineR10(code_cave, detour);
#else
		memGuard::setAllowedAccess(code_cave, sizeof(longjump_trampoline_noreg), memGuard::ACC_RWX);
		writeLongjumpTrampolineNoreg(code_cave, detour);
#endif

		createOriginal(sizeof(jmp_trampoline));
	}

	void CompactDetourHook::destroy()
	{
#if SOUP_BITS == 64
		memset(code_cave, 0xCC, sizeof(longjump_trampoline_r10));
#else
		memset(code_cave, 0xCC, sizeof(longjump_trampoline_noreg));
#endif

		destroyOriginal();
	}

	void CompactDetourHook::enable()
	{
		void* addr = getEffectiveTarget();
		memGuard::setAllowedAccess(addr, sizeof(jmp_trampoline), memGuard::ACC_RWX);
		writeJmpTrampoline(addr, code_cave);
	}

	void CompactDetourHook::disable()
	{
		memcpy(getEffectiveTarget(), original, sizeof(jmp_trampoline));
	}
}

#endif
