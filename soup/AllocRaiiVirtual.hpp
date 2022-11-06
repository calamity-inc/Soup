#pragma once

#include "AllocRaiiLocalBase.hpp"

#include "base.hpp"

#if SOUP_WINDOWS
#include <Windows.h>
#else
#include <sys/mman.h>
#endif

namespace soup
{
	struct AllocRaiiVirtual : public AllocRaiiLocalBase
	{
		enum ProtFlags : int
		{
			READ = 0x01, // equals PROT_READ on Linux
			WRITE = 0x02, // equals PROT_WRITE on Linux
			EXEC = 0x04, // equals PROT_EXEC on Linux
		};

#if SOUP_WINDOWS
		[[nodiscard]] static constexpr DWORD protFlagsToProtect(int prot_flags)
		{
			DWORD protect = 0x01; // PAGE_NOACCESS
			if (prot_flags & WRITE)
			{
				protect = 0x04; // PAGE_READWRITE
			}
			else if (prot_flags & READ)
			{
				protect = 0x02; // PAGE_READONLY
			}
			if (prot_flags & EXEC)
			{
				protect <<= 4;
				// PAGE_NOACCESS (0x01) -> PAGE_EXECUTE (0x10)
				// PAGE_READONLY (0x02) -> PAGE_EXECUTE_READ (0x20)
				// PAGE_READWRITE (0x04) -> PAGE_EXECUTE_READWRITE (0x40)
			}
			return protect;
		}
#endif

		size_t size;

		AllocRaiiVirtual(size_t size, int prot_flags = (READ | WRITE | EXEC))
			: AllocRaiiLocalBase(
#if SOUP_WINDOWS
				VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, protFlagsToProtect(prot_flags))
#else
				mmap(nullptr, size, prot_flags, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0)
#endif
			), size(size)
		{
		}

		~AllocRaiiVirtual()
		{
#if SOUP_WINDOWS
			VirtualFree(addr, size, MEM_DECOMMIT);
#else
			munmap(addr, size);
#endif
		}
	};
}
