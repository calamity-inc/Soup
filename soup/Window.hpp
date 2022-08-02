#pragma once

#include "base.hpp"
#if SOUP_WINDOWS
#include "fwd.hpp"

#include <string>
#include <vector>

#include <Windows.h>

#include "Capture.hpp"

namespace soup
{
	struct Window
	{
		using callback_t = void(*)(Window);
		using draw_func_t = void(*)(Window, RenderTarget&);
		using on_click_t = void(*)(Window, int, int);
		using mouse_informer_t = on_click_t(*)(Window, int, int);

		struct Config
		{
			Capture custom_data;
			draw_func_t draw_func = nullptr;
			bool resizable = false;
			mouse_informer_t mouse_informer = nullptr;
			callback_t on_close = nullptr;
			std::vector<callback_t> hotkey_callbacks{};
		};

		HWND h;

		[[nodiscard]] static Window create(const std::string& title, int width, int height, const std::string& icon_ico = {}) noexcept;
		[[nodiscard]] static Window getFocused() noexcept;

		[[nodiscard]] DWORD getOwnerPid() const noexcept;

		[[nodiscard]] Window::Config& getConfig();
		Capture& customData();
		Window& setDrawFunc(draw_func_t draw_func);
		Window& setMouseInformer(mouse_informer_t mouse_informer);
		Window& onClose(callback_t on_close);
		Window& registerHotkey(bool meta, bool ctrl, bool shift, bool alt, unsigned int key, callback_t callback);

		[[nodiscard]] bool getIsVisible() noexcept;
		Window& setIsVisible(bool visible) noexcept;
		Window& show() noexcept;
		Window& hide() noexcept;

		Window& setPos(int x, int y) noexcept;
		Window& setSize(int width, int height) noexcept;
		Window& setResizable(bool b) noexcept;

		Window& bringToFront() noexcept;
		Window& redraw() noexcept;

		Window& setInvisibleColour(Rgb rgb) noexcept; // Windows is weird. When the invisible colour is black or { 1, 0, 0 }, you can only drag visible pixels, but if it's red or { 1, 1, 1 }, you can drag everywhere.
		Window& setTransparency(int a) noexcept; // 0-100. 100 is fully invisible.
		Window& setTopmost(bool on) noexcept;

		static int runMessageLoop() noexcept;
		static void endMessageLoop(int exit_code = 0) noexcept;
	};
}
#endif
