#pragma once

#include <cstddef> // size_t

#include "base.hpp"

NAMESPACE_SOUP
{
	struct memGuard
	{
		enum AllowedAccessFlags : int
		{
			ACC_READ = 0x01, // equals PROT_READ on Linux
			ACC_WRITE = 0x02, // equals PROT_WRITE on Linux
			ACC_EXEC = 0x04, // equals PROT_EXEC on Linux

			ACC_RWX = ACC_READ | ACC_WRITE | ACC_EXEC
		};

		[[nodiscard]] static void* alloc(size_t len, int allowed_access);
		static void free(void* addr, size_t len);
		static void setAllowedAccess(void* addr, size_t len, int allowed_access);
	};
}
