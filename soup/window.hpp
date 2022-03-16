#pragma once

#include <Windows.h>

namespace soup
{
	struct window
	{
		HWND h;

		[[nodiscard]] static window getFocused() noexcept
		{
			return window{ GetForegroundWindow() };
		}

		[[nodiscard]] DWORD getOwnerPid()
		{
			DWORD pid;
			GetWindowThreadProcessId(h, &pid);
			return pid;
		}
	};
}
