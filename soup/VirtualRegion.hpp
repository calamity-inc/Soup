#pragma once

#include "base.hpp"
#if SOUP_WINDOWS

#include <Windows.h>

namespace soup
{
	struct VirtualRegion
	{
		size_t size{};
		BYTE* pointer{};
		BYTE* base{};

		VirtualRegion(BYTE* base, size_t size, BYTE* pointer)
			: size(size), pointer(pointer), base(base)
		{
		}

		BYTE* end() const noexcept
		{
			return base + size;
		}
	};
}
#endif
