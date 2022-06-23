#pragma once

#include <cstdint>
#include <vector>

#undef CDECL

namespace soup
{
	struct BadCall { };

	struct ffi
	{
		enum CallConv : uint8_t
		{
			CDECL = 0,
			FASTCALL,
			STDCALL,
			VECTORCALL,
		};

		static uintptr_t call(CallConv conv, void* func, const std::vector<uintptr_t>& args);

		static uintptr_t cdeclCall(void* func, const std::vector<uintptr_t>& args);
		static uintptr_t fastcall(void* func, const std::vector<uintptr_t>& args);
		static uintptr_t stdcall(void* func, const std::vector<uintptr_t>& args);
		static uintptr_t vectorcall(void* func, const std::vector<uintptr_t>& args);
	};
}
