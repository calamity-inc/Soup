#pragma once

#include "base.hpp"
#include "mouse_button.hpp"
#include "rgb.hpp"
#include "string.hpp"

#include <iostream>

#if SOUP_WINDOWS
#include <Windows.h>
#else
#include <termios.h>
#endif

#define BEL "\x7"
#define ESC "\x1B"
#define CSI ESC "["
#define OSC ESC "]"
#define ST  ESC "\\"

namespace soup
{
	class console_impl
	{
	private:
#if SOUP_WINDOWS
		bool pressed_lmb = false;
		bool pressed_rmb = false;
		bool pressed_mmb = false;
#else
		inline static struct termios termattrs_og{};
		inline static struct termios termattrs_cur{};
#endif

	public:
		using input_handler_t = void(*)(char32_t);
		using mouse_handler_t = void(*)(mouse_button, int x, int y);

		input_handler_t input_handler = nullptr;
	private:
		mouse_handler_t mouse_handler = nullptr;

	public:
		void init();
		void run();
		void cleanup();

		void onMouseClick(mouse_handler_t handler);

		/* This shit is only supported by MingW, like, why bother when you don't support anything else?!

		void hideScrollbar()
		{
			std::cout << CSI "?47h";
			std::cout << CSI "?30l";
		}

		void showScrollbar()
		{
			std::cout << CSI "?47l";
			std::cout << CSI "?30h";
		}*/

		// Window

		void getSize(int& outWidth, int& outHeight) const;
		static void setTitle(const std::string& title);

		// Output

		static void bell();
		static void clearScreen();
		static void saveCursorPos();
		static void restoreCursorPos();
		static void fillScreen(int r, int g, int b);
		static void setCursorPos(int x, int y);

		template <typename T>
		const console_impl& operator << (const T& str) const
		{
			std::cout << str;
			return *this;
		}

		template <typename T>
		const console_impl& operator << (T&& str) const
		{
			std::cout << std::move(str);
			return *this;
		}

		static void setForegroundColour(rgb c);

		static void setForegroundColour(int r, int g, int b);

		template <typename Str>
		[[nodiscard]] static Str strSetForegroundColour(int r, int g, int b)
		{
			Str str;
			str.push_back(CSI[0]);
			str.push_back(CSI[1]);
			str.push_back('3');
			str.push_back('8');
			str.push_back(';');
			str.push_back('2');
			str.push_back(';');
			str.append(string::decimal<Str>(r));
			str.push_back(';');
			str.append(string::decimal<Str>(g));
			str.push_back(';');
			str.append(string::decimal<Str>(b));
			str.push_back('m');
			return str;
		}

		static void setBackgroundColour(rgb c);

		static void setBackgroundColour(int r, int g, int b);

		template <typename Str>
		[[nodiscard]] static Str strSetBackgroundColour(int r, int g, int b)
		{
			Str str;
			str.push_back(CSI[0]);
			str.push_back(CSI[1]);
			str.push_back('4');
			str.push_back('8');
			str.push_back(';');
			str.push_back('2');
			str.push_back(';');
			str.append(string::decimal<Str>(r));
			str.push_back(';');
			str.append(string::decimal<Str>(g));
			str.push_back(';');
			str.append(string::decimal<Str>(b));
			str.push_back('m');
			return str;
		}

		static void resetColour();

		// Ctrl+C

	private:
		using ctrl_c_handler_t = void(*)();

		inline static ctrl_c_handler_t ctrl_c_handler = nullptr;

#if SOUP_WINDOWS
		static BOOL WINAPI CtrlHandler(DWORD ctrlType);
#else
		static void sigint_handler_proc(int s);
#endif

	public:
		static void overrideCtrlC(ctrl_c_handler_t handler);
	};

	inline console_impl console;
}
