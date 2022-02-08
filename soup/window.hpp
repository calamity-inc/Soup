#pragma once

#include <Windows.h>

namespace soup
{
	struct window
	{
		HWND h;

		[[nodiscard]] static window get_focused() noexcept
		{
			return window(GetForegroundWindow());
		}

		[[nodiscard]] DWORD get_owner_pid()
		{
			DWORD pid;
			GetWindowThreadProcessId(h, &pid);
			return pid;
		}
	};
}
