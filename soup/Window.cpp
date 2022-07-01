#include "Window.hpp"

#if SOUP_WINDOWS
#include <Windowsx.h> // GET_X_LPARAM, GET_Y_LPARAM

#include <unordered_map>

#include "rand.hpp"
#include "RenderTargetWindow.hpp"
#include "unicode.hpp"

namespace soup
{
	static std::unordered_map<HWND, Window::Config> window_configs{};

	LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		if (auto window_config_entry = window_configs.find(hWnd); window_config_entry != window_configs.end())
		{
			Window::Config& wc = window_config_entry->second;
			switch (message)
			{
			case WM_PAINT:
				if (wc.draw_func)
				{
					RECT rect;
					GetUpdateRect(hWnd, &rect, FALSE);

					PAINTSTRUCT ps;
					HDC hdc = BeginPaint(hWnd, &ps);
					RenderTargetWindow rt{ rect.right - rect.left, rect.bottom - rect.top, hdc };
					wc.draw_func(Window{ hWnd }, rt);
					EndPaint(hWnd, &ps);
				}
				return 0;

			case WM_NCHITTEST:
				{
					// Make window movable from any position
					LRESULT hit = DefWindowProcW(hWnd, message, wParam, lParam);
					if (hit == HTCLIENT)
					{
						POINT p;
						p.x = GET_X_LPARAM(lParam);
						p.y = GET_Y_LPARAM(lParam);
						MapWindowPoints(HWND_DESKTOP, hWnd, &p, 1);

						if (wc.resizable)
						{
							RECT r;
							GetWindowRect(hWnd, &r);
							const auto dx = ((r.right - r.left) - 5);
							const auto dy = ((r.bottom - r.top) - 5);

							if (p.x <= 5 && p.y <= 5)
							{
								hit = HTTOPLEFT;
							}
							else if (p.x >= dx && p.y <= 5)
							{
								hit = HTTOPRIGHT;
							}
							else if (p.x <= 5 && p.y >= dy)
							{
								hit = HTTOPRIGHT;
							}
							else if (p.x >= dx && p.y >= dy)
							{
								hit = HTBOTTOMRIGHT;
							}
							else if (p.x <= 5)
							{
								hit = HTLEFT;
							}
							else if (p.y <= 5)
							{
								hit = HTTOP;
							}
							else if (p.x >= dx)
							{
								hit = HTRIGHT;
							}
							else if (p.y >= dy)
							{
								hit = HTBOTTOM;
							}
						}
						if (hit == HTCLIENT
							&& (wc.mouse_informer == nullptr
								|| wc.mouse_informer(Window{ hWnd }, p.x, p.y) == nullptr
								)
							)
						{
							hit = HTCAPTION;
						}
					}
					return hit;
				}
				break;

			case WM_LBUTTONDOWN:
				if (wc.mouse_informer != nullptr)
				{
					POINT p;
					p.x = GET_X_LPARAM(lParam);
					p.y = GET_Y_LPARAM(lParam);
					if (auto on_click = wc.mouse_informer(Window{ hWnd }, p.x, p.y))
					{
						on_click(Window{ hWnd }, p.x, p.y);
					}
				}
				return 0;

			case WM_GETMINMAXINFO:
				{
					LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
					lpMMI->ptMinTrackSize.x = 15;
					lpMMI->ptMinTrackSize.y = 15;
				}
				return 0;

			case WM_HOTKEY:
				if (wParam >= 0)
				{
					wc.hotkey_callbacks.at(wParam)(Window{ hWnd });
				}
				return 0;

			case WM_DESTROY:
				if (wc.on_close)
				{
					wc.on_close(Window{ hWnd });
				}
				window_configs.erase(window_config_entry);
				return 0;
			}
		}
		return DefWindowProcW(hWnd, message, wParam, lParam);
	}

	Window Window::create(const std::string& title, int width, int height, const std::string& icon_ico) noexcept
	{
		HINSTANCE hInstance = GetModuleHandle(NULL);

		std::wstring menu_name = unicode::utf8_to_utf16(title);

		std::wstring class_name = L"SOUP_";
		class_name.append(rand.str<std::wstring>(5));

		WNDCLASSEXW wcex{};
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = &WndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = hInstance;
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wcex.lpszMenuName = menu_name.c_str();
		wcex.lpszClassName = class_name.c_str();
		if (!icon_ico.empty())
		{
			std::wstring icon_ico_w = unicode::utf8_to_utf16(icon_ico);
			wcex.hIconSm = wcex.hIcon = (HICON)LoadImageW(NULL, icon_ico_w.c_str(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);
		}
		RegisterClassExW(&wcex);

		HWND hWnd = CreateWindowW(wcex.lpszClassName, wcex.lpszMenuName, 0, CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, hInstance, nullptr);
		SetWindowLong(hWnd, GWL_STYLE, 0);
		window_configs.emplace(hWnd, Window::Config{});
		return Window{ hWnd };
	}

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

	Window::Config& Window::getConfig()
	{
		return window_configs.at(h);
	}

	Capture& Window::getCustomData()
	{
		return getConfig().custom_data;
	}

	Window& Window::setDrawFunc(draw_func_t draw_func)
	{
		getConfig().draw_func = draw_func;
		return *this;
	}

	Window& Window::setMouseInformer(mouse_informer_t mouse_informer)
	{
		getConfig().mouse_informer = mouse_informer;
		return *this;
	}

	Window& Window::onClose(callback_t on_close)
	{
		getConfig().on_close = on_close;
		return *this;
	}

	Window& Window::registerHotkey(bool meta, bool ctrl, bool shift, bool alt, unsigned int key, callback_t callback)
	{
		unsigned int mod = alt;
		mod |= (ctrl << 1);
		mod |= (shift << 2);
		mod |= (meta << 3);

		Window::Config& wc = getConfig();
		RegisterHotKey(h, (int)wc.hotkey_callbacks.size(), mod, key);
		wc.hotkey_callbacks.emplace_back(callback);

		return *this;
	}

	bool Window::getIsVisible() noexcept
	{
		return IsWindowVisible(h);
	}

	Window& Window::setIsVisible(bool show) noexcept
	{
		ShowWindow(h, show * SW_SHOW);
		return *this;
	}

	Window& Window::show() noexcept
	{
		ShowWindow(h, SW_SHOW);
		return *this;
	}

	Window& Window::hide() noexcept
	{
		ShowWindow(h, SW_HIDE);
		return *this;
	}

	Window& Window::setPos(int x, int y) noexcept
	{
		SetWindowPos(h, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
		return *this;
	}

	Window& Window::setSize(int width, int height) noexcept
	{
		SetWindowPos(h, 0, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
		return *this;
	}

	Window& Window::setResizable(bool b) noexcept
	{
		getConfig().resizable = b;
		return *this;
	}

	Window& Window::bringToFront() noexcept
	{
		SetForegroundWindow(h);
		return *this;
	}

	Window& Window::redraw() noexcept
	{
		InvalidateRect(h, NULL, FALSE);
		return *this;
	}

	Window& Window::setExitOnClose() noexcept
	{
		onClose([](Window w)
		{
			endMessageLoop();
		});
		return *this;
	}

	Window& Window::setInvisibleColour(Rgb rgb) noexcept
	{
		SetWindowLong(h, GWL_EXSTYLE, GetWindowLong(h, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(h, RGB(rgb.r, rgb.g, rgb.b), 0, LWA_COLORKEY);
		return *this;
	}

	Window& Window::setTransparency(int a) noexcept
	{
		SetWindowLong(h, GWL_EXSTYLE, GetWindowLong(h, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(h, 0, a, LWA_ALPHA);
		return *this;
	}

	Window& Window::setTopmost(bool on) noexcept
	{
		if (on)
		{
			SetWindowPos(h, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		else
		{
			SetWindowPos(h, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		return *this;
	}

	int Window::runMessageLoop() noexcept
	{
		MSG msg;
		while (GetMessage(&msg, nullptr, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return (int)msg.wParam;
	}

	void Window::endMessageLoop(int exit_code) noexcept
	{
		PostQuitMessage(exit_code);
	}
}
#endif
