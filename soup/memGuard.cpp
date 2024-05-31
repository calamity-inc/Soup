#include "memGuard.hpp"

#if SOUP_WINDOWS
	#include <windows.h>
#else
	#include <sys/mman.h>
#endif

NAMESPACE_SOUP
{
#if SOUP_WINDOWS
	[[nodiscard]] static DWORD allowedAccessToProtect(int allowed_access)
	{
		DWORD protect = 0x01; // PAGE_NOACCESS
		if (allowed_access & memGuard::ACC_WRITE)
		{
			protect = 0x04; // PAGE_READWRITE
		}
		else if (allowed_access & memGuard::ACC_READ)
		{
			protect = 0x02; // PAGE_READONLY
		}
		if (allowed_access & memGuard::ACC_EXEC)
		{
			protect <<= 4;
			// PAGE_NOACCESS (0x01) -> PAGE_EXECUTE (0x10)
			// PAGE_READONLY (0x02) -> PAGE_EXECUTE_READ (0x20)
			// PAGE_READWRITE (0x04) -> PAGE_EXECUTE_READWRITE (0x40)
		}
		return protect;
	}
#endif

	void* memGuard::alloc(size_t len, int allowed_access)
	{
#if SOUP_WINDOWS
		return VirtualAlloc(nullptr, len, MEM_COMMIT | MEM_RESERVE, allowedAccessToProtect(allowed_access));
#else
		return mmap(nullptr, len, allowed_access, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
#endif
	}

	void memGuard::free(void* addr, size_t len)
	{
#if SOUP_WINDOWS
		VirtualFree(addr, len, MEM_DECOMMIT);
#else
		munmap(addr, len);
#endif
	}

	void memGuard::setAllowedAccess(void* addr, size_t len, int allowed_access)
	{
#if SOUP_WINDOWS
		DWORD oldprotect;
		VirtualProtect(addr, len, allowedAccessToProtect(allowed_access), &oldprotect);
#else
		mprotect(addr, len, allowed_access);
#endif
	}
}
