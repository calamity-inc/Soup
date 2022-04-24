#pragma once

#include "base.hpp"

#if SOUP_WINDOWS
#include <Windows.h>

namespace soup
{
	struct Window
	{
		HWND h;

		[[nodiscard]] static Window getFocused() noexcept;

		[[nodiscard]] DWORD getOwnerPid() const noexcept;
	};
}
#endif
