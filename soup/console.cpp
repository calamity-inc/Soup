#include "console.hpp"

#include <thread>

#include "unicode.hpp"

#if SOUP_WINDOWS
#pragma comment(lib, "User32.lib") // GetAsyncKeyState

#include <conio.h>
#else
#include <fcntl.h> // open
#include <signal.h>
#include <termios.h>
#include <unistd.h> // read, close
#endif

namespace soup
{
	void console_impl::init()
	{
#if SOUP_WINDOWS
		if (auto hSTDOUT = GetStdHandle(STD_OUTPUT_HANDLE);
			hSTDOUT != INVALID_HANDLE_VALUE
			)
		{
			DWORD mode;
			if (GetConsoleMode(hSTDOUT, &mode)
				&& SetConsoleMode(hSTDOUT, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING)
				)
			{
				owner_of_console = true;
			}
		}

		// Set console to UTF-8, MingW is UTF-8 regardless.
		SetConsoleCP(CP_UTF8);
		SetConsoleOutputCP(CP_UTF8);
#else
		tcgetattr(0, &termattrs_og);
		termattrs_cur = termattrs_og;
#endif
	}

	void console_impl::run()
	{
		std::string utf8_buf{};
#if SOUP_WINDOWS
		int8_t mingw_pending_mb = -1;
		int mingw_pending_mb_x;

		DWORD cNumRead;
		INPUT_RECORD irInBuf[128];
		while (ReadConsoleInputA(GetStdHandle(STD_INPUT_HANDLE), irInBuf, 128, &cNumRead))
		{
			for (DWORD i = 0; i < cNumRead; i++)
			{
				switch (irInBuf[i].EventType)
				{
				case KEY_EVENT:
					if (irInBuf[i].Event.KeyEvent.bKeyDown)
					{
						if (irInBuf[i].Event.KeyEvent.uChar.UnicodeChar != 0)
						{
							if (mouse_handler)
							{
								if (mingw_pending_mb != -1)
								{
									int val = irInBuf[i].Event.KeyEvent.uChar.UnicodeChar - 32;
									if (mingw_pending_mb_x != -1)
									{
										if (mouse_handler != nullptr)
										{
											mouse_handler((mouse_button)mingw_pending_mb, mingw_pending_mb_x, val);
										}
										mingw_pending_mb = -1;
									}
									else
									{
										mingw_pending_mb_x = val;
									}
									continue;
								}
								if (irInBuf[i].Event.KeyEvent.uChar.UnicodeChar == 32)
								{
									if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
									{
										mingw_pending_mb = LMB;
										mingw_pending_mb_x = -1;
									}
									continue;
								}
								if (irInBuf[i].Event.KeyEvent.uChar.UnicodeChar == 33)
								{
									if (GetAsyncKeyState(VK_MBUTTON) & 0x8000)
									{
										mingw_pending_mb = MMB;
										mingw_pending_mb_x = -1;
									}
									continue;
								}
								if (irInBuf[i].Event.KeyEvent.uChar.UnicodeChar == 34)
								{
									if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
									{
										mingw_pending_mb = RMB;
										mingw_pending_mb_x = -1;
									}
									continue;
								}
							}
							if (input_handler != nullptr)
							{
								utf8_buf.push_back((char)irInBuf[i].Event.KeyEvent.uChar.UnicodeChar);
								auto it = utf8_buf.cbegin();
								const auto end = utf8_buf.cend();
								char32_t uni = unicode::utf8_to_utf32_char(it, end);
								if (uni != 0)
								{
									utf8_buf.clear();
									input_handler(uni);
								}
							}
							continue;
						}
						// Handle non-char keys here, e.g. arrow keys
						/*if (input_handler != nullptr)
						{
							input_handler(irInBuf[i].Event.KeyEvent.wVirtualKeyCode * -1);
						}*/
					}
					break;

				case MOUSE_EVENT:
					if (irInBuf[i].Event.MouseEvent.dwEventFlags == 0)
					{
						if (mouse_handler != nullptr)
						{
							bool lmb = (irInBuf[i].Event.MouseEvent.dwButtonState & 0x1);
							bool rmb = (irInBuf[i].Event.MouseEvent.dwButtonState & 0x2);
							bool mmb = (irInBuf[i].Event.MouseEvent.dwButtonState & 0x4);
							if ((pressed_lmb ^ lmb) && (pressed_lmb = lmb))
							{
								mouse_handler(LMB, irInBuf[i].Event.MouseEvent.dwMousePosition.X + 1, irInBuf[i].Event.MouseEvent.dwMousePosition.Y);
							}
							if ((pressed_rmb ^ rmb) && (pressed_rmb = rmb))
							{
								mouse_handler(RMB, irInBuf[i].Event.MouseEvent.dwMousePosition.X + 1, irInBuf[i].Event.MouseEvent.dwMousePosition.Y);
							}
							if ((pressed_mmb ^ mmb) && (pressed_mmb = mmb))
							{
								mouse_handler(MMB, irInBuf[i].Event.MouseEvent.dwMousePosition.X + 1, irInBuf[i].Event.MouseEvent.dwMousePosition.Y);
							}
						}
					}
					break;
				}
			}
		}
#else
		std::cout << std::flush;

		termattrs_cur.c_lflag &= ~ICANON; // no buffered i/o
		termattrs_cur.c_lflag &= ~ECHO; // no echo
		tcsetattr(0, TCSANOW, &termattrs_cur);

		auto in = open("/dev/stdin", O_RDONLY | O_NONBLOCK);
		while (true)
		{
			char c;
			if (read(in, &c, 1) != 1)
			{
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(1ms);
				continue;
			}
			if (c == ESC[0])
			{
				char _;
				if (read(in, &_, 1) == 1)
				{
					if (_ == '[') // CSI
					{
						if (read(in, &_, 1) == 1)
						{
							if (_ == 'M')
							{
								char mouse_event_args[3];
								if (read(in, mouse_event_args, 3) == 3)
								{
									if (mouse_handler)
									{
										auto button_info = (mouse_event_args[0] & 0b11);
										if (button_info == 0)
										{
											mouse_handler(LMB, mouse_event_args[1] - 32, mouse_event_args[2] - 32);
											std::cout << std::flush;
										}
										else if (button_info == 1)
										{
											mouse_handler(MMB, mouse_event_args[1] - 32, mouse_event_args[2] - 32);
											std::cout << std::flush;
										}
										else if (button_info == 2)
										{
											mouse_handler(RMB, mouse_event_args[1] - 32, mouse_event_args[2] - 32);
											std::cout << std::flush;
										}
									}
								}
							}
						}
					}
					continue;
				}
			}
			if (input_handler != nullptr)
			{
				utf8_buf.push_back(c);
				auto it = utf8_buf.cbegin();
				const auto end = utf8_buf.cend();
				char32_t uni = unicode::utf8_to_utf32_char(it, end);
				if (uni != 0)
				{
					utf8_buf.clear();
					input_handler(uni);
					std::cout << std::flush;
				}
			}
		}
		close(in);
#endif
	}

	void console_impl::cleanup()
	{
		resetColour();
		clearScreen();
		setCursorPos(1, 1);

#if !SOUP_WINDOWS
		tcsetattr(0, TCSANOW, &termattrs_og);
#endif

		if (mouse_handler)
		{
#if SOUP_WINDOWS
			std::cout << CSI "?9l";
#else
			std::cout << CSI "?1000l";
#endif
			mouse_handler = nullptr;
		}
	}

	void console_impl::onMouseClick(mouse_handler_t handler)
	{
		if (!mouse_handler)
		{
#if SOUP_WINDOWS
			if (auto hSTDIN = GetStdHandle(STD_INPUT_HANDLE);
				hSTDIN != INVALID_HANDLE_VALUE
				)
			{
				DWORD mode;
				if (GetConsoleMode(hSTDIN, &mode))
				{
					mode |= ENABLE_MOUSE_INPUT;
					mode &= ~ENABLE_QUICK_EDIT_MODE;
					mode |= ENABLE_EXTENDED_FLAGS;
					SetConsoleMode(hSTDIN, mode);
				}
			}

			std::cout << CSI "?9h";
#else
			std::cout << CSI "?1000h";
#endif
		}
		mouse_handler = handler;
	}

	void console_impl::getSize(int& outWidth, int& outHeight) const
	{
#if SOUP_WINDOWS
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		outWidth = csbi.dwSize.X;
		outHeight = csbi.dwSize.Y;
#else
		// TODO
		//std::cout << CSI "18t";
#endif
	}

	void console_impl::setTitle(const std::string& title)
	{
		std::cout << OSC "2;" << title << ST;
	}

	void console_impl::bell()
	{
		std::cout << BEL;
	}

	void console_impl::clearScreen()
	{
		std::cout << CSI "2J";
	}

	void console_impl::saveCursorPos()
	{
		std::cout << CSI "s";
	}

	void console_impl::restoreCursorPos()
	{
		std::cout << CSI "u";
	}

	void console_impl::fillScreen(int r, int g, int b)
	{
		setBackgroundColour(r, g, b);
		clearScreen();
	}

	void console_impl::setCursorPos(int x, int y)
	{
		std::cout << CSI << y << ";" << x << "H";
	}

	void console_impl::setForegroundColour(rgb c)
	{
		return setForegroundColour(c.r, c.g, c.b);
	}

	void console_impl::setForegroundColour(int r, int g, int b)
	{
		std::cout << CSI "38;2;" << r << ";" << g << ";" << b << "m";
	}

	void console_impl::setBackgroundColour(rgb c)
	{
		return setBackgroundColour(c.r, c.g, c.b);
	}

	void console_impl::setBackgroundColour(int r, int g, int b)
	{
		std::cout << CSI "48;2;" << r << ";" << g << ";" << b << "m";
	}

	void console_impl::resetColour()
	{
		std::cout << CSI "m";
	}

#if SOUP_WINDOWS
	BOOL WINAPI console_impl::CtrlHandler(DWORD ctrlType)
	{
		if (ctrlType == CTRL_C_EVENT)
		{
			ctrl_c_handler();
			return TRUE;
		}
		return FALSE;
	}
#else
	void console_impl::sigint_handler_proc(int s)
	{
		if (!ctrl_c_handler)
		{
			exit(0);
		}
		ctrl_c_handler();
	}
#endif

	void console_impl::overrideCtrlC(ctrl_c_handler_t handler)
	{
		if (!ctrl_c_handler)
		{
#if SOUP_WINDOWS
			SetConsoleCtrlHandler(&CtrlHandler, TRUE);
#else
			struct sigaction sigint_handler;
			sigint_handler.sa_handler = &sigint_handler_proc;
			sigemptyset(&sigint_handler.sa_mask);
			sigint_handler.sa_flags = 0;
			sigaction(SIGINT, &sigint_handler, NULL);
#endif
		}
		ctrl_c_handler = handler;
	}
}
