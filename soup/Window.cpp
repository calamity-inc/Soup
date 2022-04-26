#include "Window.hpp"

#if SOUP_WINDOWS

namespace soup
{
    Window Window::getFocused() noexcept
    {
		return Window{ GetForegroundWindow() };
    }

    DWORD Window::getOwnerPid() const noexcept
    {
		DWORD pid;
		GetWindowThreadProcessId(h, &pid);
		return pid;
    }
}
#endif
