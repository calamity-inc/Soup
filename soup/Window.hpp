#pragma once

#include "base.hpp"
#include "fwd.hpp"

#include <string>
#include <vector>

#if SOUP_WINDOWS
#include <Windows.h>
#else
#include "X11Api.hpp"
#endif

#include "Capture.hpp"

namespace soup
{
	struct Window
	{
#if SOUP_WINDOWS
		using handle_t = HWND;
#else
		using handle_t = X11Api::Window;
#endif

#if SOUP_WINDOWS
		using callback_t = void(*)(Window);
		using draw_func_t = void(*)(Window, RenderTarget&);
		using on_click_t = void(*)(Window, unsigned int, unsigned int);
		using mouse_informer_t = on_click_t(*)(Window, unsigned int, unsigned int);
		using char_callback_t = void(*)(Window, char32_t);
		using key_callback_t = void(*)(Window, char32_t, bool down, bool repeat);
#endif

		struct Config
		{
			Capture custom_data;
#if SOUP_WINDOWS
			draw_func_t draw_func = nullptr;
			bool resizable = false;
			mouse_informer_t mouse_informer = nullptr;
			char_callback_t char_callback = nullptr;
			key_callback_t key_callback = nullptr;
			std::vector<callback_t> hotkey_callbacks{};
			callback_t on_focus = nullptr;
			callback_t on_blur = nullptr;
			callback_t on_close = nullptr;
#endif
		};

		handle_t h = 0;

		operator bool() const noexcept
		{
			return h != 0;
		}

		void reset() noexcept
		{
			h = 0;
		}

#if SOUP_WINDOWS
		[[nodiscard]] static Window create(const std::string& title, unsigned int width, unsigned int height, const std::string& icon_ico) noexcept;
		[[nodiscard]] static Window create(const std::string& title, unsigned int width, unsigned int height, HICON icon = NULL) noexcept;
#else
		[[nodiscard]] static Window create(const std::string& title, unsigned int width, unsigned int height) noexcept;
#endif
#if SOUP_WINDOWS
		[[nodiscard]] static Window getFocused() noexcept;

		[[nodiscard]] DWORD getOwnerPid() const noexcept;
		[[nodiscard]] DWORD getOwnerTid() const noexcept;
#endif

		[[nodiscard]] Window::Config& getConfig();
		Capture& customData();
#if SOUP_WINDOWS
		Window& setDrawFunc(draw_func_t draw_func);
		Window& setMouseInformer(mouse_informer_t mouse_informer);
		Window& setCharCallback(char_callback_t char_callback);
		Window& setKeyCallback(key_callback_t key_callback);
		Window& registerHotkey(bool meta, bool ctrl, bool shift, bool alt, unsigned int key, callback_t callback);
		Window& onFocus(callback_t on_focus);
		Window& onBlur(callback_t on_blur);
		Window& onClose(callback_t on_close);

		[[nodiscard]] bool getIsVisible() noexcept;
		Window& setIsVisible(bool visible) noexcept;
		Window& show() noexcept;
		Window& hide() noexcept;

		Window& hideFromTaskbar() noexcept;

		[[nodiscard]] std::pair<unsigned int, unsigned int> getPos() noexcept;
		[[nodiscard]] std::pair<unsigned int, unsigned int> getSize() noexcept;
		Window& setPos(int x, int y) noexcept;
		Window& setSize(int width, int height) noexcept;
		Window& setResizable(bool b) noexcept;

		Window& bringToFront() noexcept;
		Window& redraw() noexcept;

		Window& setInvisibleColour(Rgb rgb) noexcept; // Windows is weird. When the invisible colour is black or { 1, 0, 0 }, you can only drag visible pixels, but if it's red or { 1, 1, 1 }, you can drag everywhere.
		Window& setTransparency(int a) noexcept; // 0-100. 100 is fully invisible.
		Window& setTopmost(bool on) noexcept;
#endif

		static void runMessageLoop() noexcept;
#if SOUP_WINDOWS
		void close() noexcept;
#endif
	};
}
