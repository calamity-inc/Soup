#pragma once

#include "base.hpp"

#if SOUP_WINDOWS
#include <Windows.h>

namespace soup
{
	struct window
	{
		HWND h;

		[[nodiscard]] static window getFocused() noexcept;

		[[nodiscard]] DWORD getOwnerPid() const noexcept;
	};
}
#endif
