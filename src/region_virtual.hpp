#pragma once

#include "platform.hpp"
#if SOUP_PLATFORM_WINDOWS

#include <Windows.h>

namespace soup
{
	struct region_virtual
	{
		size_t size{};
		BYTE* pointer{};
		BYTE* base{};

		region_virtual(BYTE* base, size_t size, BYTE* pointer)
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
