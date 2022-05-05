#pragma once

#include "base.hpp"

#if SOUP_WINDOWS
#include <string>
#include <vector>

#include <Windows.h>

namespace soup
{
	struct Window
	{
		using draw_func_t = void(*)(Window, HDC);
		using on_close_t = void(*)(Window);
		using hotkey_callback_t = void(*)(Window);

		struct Config
		{
			draw_func_t draw_func = nullptr;
			on_close_t on_close = nullptr;
			std::vector<hotkey_callback_t> hotkey_callbacks{};
		};

		HWND h;

		[[nodiscard]] static Window create(const std::string& title, int width, int height, const std::string& icon_ico = {}) noexcept;
		[[nodiscard]] static Window getFocused() noexcept;

		[[nodiscard]] DWORD getOwnerPid() const noexcept;

		[[nodiscard]] Window::Config& getConfig();
		Window& setDrawFunc(draw_func_t draw_func);
		Window& onClose(on_close_t on_close);
		Window& registerHotkey(bool meta, bool ctrl, bool shift, bool alt, unsigned int key, hotkey_callback_t callback);

		[[nodiscard]] bool getIsVisible() noexcept;
		Window& setIsVisible(bool visible) noexcept;
		Window& show() noexcept;
		Window& hide() noexcept;

		Window& bringToFront() noexcept;
		Window& redraw() noexcept;
		Window& setBorderless() noexcept;
		Window& setExitOnClose() noexcept;

		static int runMessageLoop() noexcept;
		static void endMessageLoop(int exit_code = 0) noexcept;
	};
}
#endif
