#pragma once

#include <cstdint>
#include <vector>

#include "Exception.hpp"

NAMESPACE_SOUP
{
	// Raised if args.size() > 20
	struct BadCall : public Exception
	{
		BadCall()
			: Exception("Bad call")
		{
		}
	};

	struct ffi
	{
		[[nodiscard]] static bool isSafeToCall(void* func) noexcept;

		static uintptr_t call(void* func, const std::vector<uintptr_t>& args);

		[[deprecated]] static uintptr_t fastcall(void* func, const std::vector<uintptr_t>& args)
		{
			return call(func, args);
		}
	};
}
