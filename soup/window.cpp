#include "window.hpp"

namespace soup
{
    window window::getFocused() noexcept
    {
		return window{ GetForegroundWindow() };
    }

    DWORD window::getOwnerPid() const noexcept
    {
		DWORD pid;
		GetWindowThreadProcessId(h, &pid);
		return pid;
    }
}
