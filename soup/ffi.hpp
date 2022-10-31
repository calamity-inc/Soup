#pragma once

#include <cstdint>
#include <vector>

#include "Exception.hpp"

#undef CDECL

namespace soup
{
	struct BadCall : public Exception
	{
		BadCall()
			: Exception("Bad call")
		{
		}
	};

	struct ffi
	{
		enum CallConv : uint8_t
		{
			CDECL = 0,
			FASTCALL,
			STDCALL,
			THISCALL,
			VECTORCALL,
		};

		static uintptr_t call(CallConv conv, void* func, const std::vector<uintptr_t>& args);

		static uintptr_t cdeclCall(void* func, const std::vector<uintptr_t>& args);
		static uintptr_t fastcall(void* func, const std::vector<uintptr_t>& args);
		static uintptr_t stdcall(void* func, const std::vector<uintptr_t>& args);
		static uintptr_t thiscall(void* func, const std::vector<uintptr_t>& args);
		static uintptr_t vectorcall(void* func, const std::vector<uintptr_t>& args);
	};
}
