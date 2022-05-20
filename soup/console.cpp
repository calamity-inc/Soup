#include "console.hpp"

#if !SOUP_WASM

#include <thread>

#include "ControlInput.hpp"
#include "MouseButton.hpp"
#include "unicode.hpp"

#if SOUP_WINDOWS
#pragma comment(lib, "User32.lib") // GetAsyncKeyState

#include <conio.h>
#else
#include <fcntl.h> // open
#include <termios.h>
#include <unistd.h> // read, close

#include "signal.hpp"
#endif

namespace soup
{
	void console_impl::init(bool fullscreen)
	{
#if SOUP_WINDOWS
		if (auto hSTDOUT = GetStdHandle(STD_OUTPUT_HANDLE);
			hSTDOUT != INVALID_HANDLE_VALUE
			)
		{
			DWORD mode;
			if (GetConsoleMode(hSTDOUT, &mode))
			{
				SetConsoleMode(hSTDOUT, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING | ENABLE_WINDOW_INPUT);
			}
		}

		// Set console to UTF-8, MingW is UTF-8 regardless.
		SetConsoleCP(CP_UTF8);
		SetConsoleOutputCP(CP_UTF8);
#else
		tcgetattr(0, &termattrs_og);
		termattrs_cur = termattrs_og;
#endif

		if (fullscreen)
		{
			std::cout << CSI "?1049h";
		}
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
							if (mingw_pending_mb != -1)
							{
								int val = irInBuf[i].Event.KeyEvent.uChar.UnicodeChar - 33;
								if (mingw_pending_mb_x != -1)
								{
									if (mouse_click_handler)
									{
										mouse_click_handler((MouseButton)mingw_pending_mb, mingw_pending_mb_x, val);
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
							if (char_handler)
							{
								utf8_buf.push_back((char)irInBuf[i].Event.KeyEvent.uChar.UnicodeChar);
								auto it = utf8_buf.cbegin();
								const auto end = utf8_buf.cend();
								char32_t uni = unicode::utf8_to_utf32_char(it, end);
								if (uni != 0)
								{
									utf8_buf.clear();
									switch (uni)
									{
									default:
										char_handler(std::move(uni));
										break;

									case 8: // cmd
									case 127: // putty
										if (control_handler)
										{
											control_handler(BACKSPACE);
										}
										break;

									case 10: // putty
									case 13: // cmd
										if (control_handler)
										{
											control_handler(NEW_LINE);
										}
										break;
									}
								}
							}
							continue;
						}
						switch (irInBuf[i].Event.KeyEvent.wVirtualKeyCode)
						{
						case VK_UP:
							if (control_handler)
							{
								control_handler(UP);
							}
							break;

						case VK_DOWN:
							if (control_handler)
							{
								control_handler(DOWN);
							}
							break;

						case VK_LEFT:
							if (control_handler)
							{
								control_handler(LEFT);
							}
							break;

						case VK_RIGHT:
							if (control_handler)
							{
								control_handler(RIGHT);
							}
							break;

						/*default:
							if (char_handler)
							{
								char_handler(irInBuf[i].Event.KeyEvent.wVirtualKeyCode * -1);
							}
							break;*/
						}
					}
					break;

				case MOUSE_EVENT:
					if ((irInBuf[i].Event.MouseEvent.dwEventFlags & ~DOUBLE_CLICK) == 0)
					{
						if (mouse_click_handler)
						{
							bool lmb = (irInBuf[i].Event.MouseEvent.dwButtonState & 0x1);
							bool rmb = (irInBuf[i].Event.MouseEvent.dwButtonState & 0x2);
							bool mmb = (irInBuf[i].Event.MouseEvent.dwButtonState & 0x4);
							if ((pressed_lmb ^ lmb) && (pressed_lmb = lmb))
							{
								mouse_click_handler(LMB, irInBuf[i].Event.MouseEvent.dwMousePosition.X, irInBuf[i].Event.MouseEvent.dwMousePosition.Y);
							}
							if ((pressed_rmb ^ rmb) && (pressed_rmb = rmb))
							{
								mouse_click_handler(RMB, irInBuf[i].Event.MouseEvent.dwMousePosition.X, irInBuf[i].Event.MouseEvent.dwMousePosition.Y);
							}
							if ((pressed_mmb ^ mmb) && (pressed_mmb = mmb))
							{
								mouse_click_handler(MMB, irInBuf[i].Event.MouseEvent.dwMousePosition.X, irInBuf[i].Event.MouseEvent.dwMousePosition.Y);
							}
						}
					}
					break;

				case WINDOW_BUFFER_SIZE_EVENT:
					if (size_handler)
					{
						size_handler(irInBuf[i].Event.WindowBufferSizeEvent.dwSize.X, irInBuf[i].Event.WindowBufferSizeEvent.dwSize.Y);
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
							if (_ == 'A') // cursor up
							{
								if (control_handler)
								{
									control_handler(UP);
								}
							}
							else if (_ == 'B') // cursor down
							{
								if (control_handler)
								{
									control_handler(DOWN);
								}
							}
							else if (_ == 'C') // cursor right
							{
								if (control_handler)
								{
									control_handler(RIGHT);
								}
							}
							else if (_ == 'D') // cursor left
							{
								if (control_handler)
								{
									control_handler(LEFT);
								}
							}
							else if (_ == 'M') // mouse event
							{
								char mouse_event_args[3];
								if (read(in, mouse_event_args, 3) == 3)
								{
									if (mouse_click_handler)
									{
										auto button_info = (mouse_event_args[0] & 0b11);
										if (button_info == 0)
										{
											mouse_click_handler(LMB, mouse_event_args[1] - 33, mouse_event_args[2] - 33);
											std::cout << std::flush;
										}
										else if (button_info == 1)
										{
											mouse_click_handler(MMB, mouse_event_args[1] - 33, mouse_event_args[2] - 33);
											std::cout << std::flush;
										}
										else if (button_info == 2)
										{
											mouse_click_handler(RMB, mouse_event_args[1] - 33, mouse_event_args[2] - 33);
											std::cout << std::flush;
										}
									}
								}
							}
							else if (_ == '8') // size
							{
								if (read(in, &_, 1) == 1)
								{
									if (_ == ';')
									{
										unsigned int height = 0;
										while (read(in, &_, 1) == 1)
										{
											unsigned int digit = (_ - '0');
											if (digit >= 10) // terminates at ';'
											{
												break;
											}
											height *= 10;
											height += digit;
										}
										unsigned int width = 0;
										while (read(in, &_, 1) == 1)
										{
											unsigned int digit = (_ - '0');
											if (digit >= 10) // terminates at 't'
											{
												break;
											}
											width *= 10;
											width += digit;
										}
										if (size_handler)
										{
											size_handler(std::move(width), std::move(height));
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
			utf8_buf.push_back(c);
			auto it = utf8_buf.cbegin();
			const auto end = utf8_buf.cend();
			char32_t uni = unicode::utf8_to_utf32_char(it, end);
			if (uni != 0)
			{
				utf8_buf.clear();
				switch (uni)
				{
				default:
					if (char_handler)
					{
						char_handler(std::move(uni));
					}
					break;

				case 8: // cmd
				case 127: // putty
					if (control_handler)
					{
						control_handler(BACKSPACE);
					}
					break;

				case 10: // putty
				case 13: // cmd
					if (control_handler)
					{
						control_handler(NEW_LINE);
					}
					break;
				}
				std::cout << std::flush;
			}
		}
		close(in);
#endif
	}

	void console_impl::cleanup()
	{
		resetColour();
		clearScreen();
		setCursorPos(0, 0);

#if !SOUP_WINDOWS
		tcsetattr(0, TCSANOW, &termattrs_og);
#endif

		if (mouse_click_handler)
		{
#if SOUP_WINDOWS
			std::cout << CSI "?9l";
#else
			std::cout << CSI "?1000l";
#endif
			mouse_click_handler.reset();
		}

		// Disable alternative screen buffer
		std::cout << CSI "?1049l";
	}

	void console_impl::onMouseClick(void(*fp)(MouseButton, unsigned int, unsigned int, const Capture&), Capture&& cap)
	{
		if (!mouse_click_handler)
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
		mouse_click_handler.set(fp, std::move(cap));
	}

	void console_impl::setTitle(const std::string& title)
	{
		// BUG: PuTTY's title will never be restored
		std::cout << OSC "2;" << title << ST;
	}

#if SOUP_LINUX
	void console_impl::sigwinch_handler_proc(int)
	{
		std::cout << CSI "18t";
		std::cout << std::flush;
	}
#endif

	void console_impl::enableSizeTracking(void(*fp)(unsigned int, unsigned int, const Capture&), Capture&& cap)
	{
		size_handler.set(fp, std::move(cap));
#if SOUP_WINDOWS
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		size_handler(csbi.dwSize.X, csbi.dwSize.Y);
#else
		std::cout << CSI "18t";
		signal::handle(SIGWINCH, &sigwinch_handler_proc);
#endif
	}

	void console_impl::bell()
	{
		std::cout << BEL;
	}

	void console_impl::clearScreen()
	{
		std::cout << CSI "2J";
	}

	void console_impl::hideCursor()
	{
		std::cout << CSI "?25l";
	}

	void console_impl::showCursor()
	{
		std::cout << CSI "?25h";
	}

	void console_impl::saveCursorPos()
	{
		std::cout << CSI "s";
	}

	void console_impl::restoreCursorPos()
	{
		std::cout << CSI "u";
	}

	void console_impl::fillScreen(Rgb c)
	{
		return fillScreen(c.r, c.g, c.b);
	}

	void console_impl::fillScreen(unsigned int r, unsigned int g, unsigned int b)
	{
		setBackgroundColour(r, g, b);
		clearScreen();
	}

	void console_impl::setCursorPos(unsigned int x, unsigned int y)
	{
		std::cout << CSI << (y + 1) << ";" << (x + 1) << "H";
	}

	const console_impl& console_impl::operator<<(const std::u16string& str) const
	{
		return *this << unicode::utf16_to_utf8(str);
	}

	void console_impl::setForegroundColour(Rgb c)
	{
		return setForegroundColour(c.r, c.g, c.b);
	}

	void console_impl::setForegroundColour(int r, int g, int b)
	{
		std::cout << CSI "38;2;" << r << ";" << g << ";" << b << "m";
	}

	void console_impl::setBackgroundColour(Rgb c)
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
	void console_impl::sigint_handler_proc(int)
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
			signal::handle(SIGINT, &sigint_handler_proc);
#endif
		}
		ctrl_c_handler = handler;
	}
}

#endif
