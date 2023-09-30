#include "cli.hpp"

#include <iostream>
#include <thread>

#include <AnalogueKeyboard.hpp>
#include <RasterFont.hpp>
#include <RenderTarget.hpp>
#include <Rgb.hpp>
#include <Thread.hpp>
#include <visKeyboard.hpp>
#include <Window.hpp>

#if SOUP_WINDOWS
using namespace soup;

static bool running = true;
static const char* kbd_name = "Digital Keyboard";
static Window w;
static visKeyboard viskbd;
#endif

void cli_keyboard()
{
#if SOUP_WINDOWS
	Thread t([]
	{
#if true
		for (const auto& kbd : AnalogueKeyboard::getAll())
		{
			kbd_name = kbd.name;
			w.redraw();
			while (running)
			{
				auto keys = kbd.getActiveKeys();
				viskbd.clear();
				for (const auto& key : keys)
				{
					//std::cout << std::hex << "Scancode " << (int)key.scancode << ", VK " << key.getVk() << "\n";
					if (key.hasUsbHidScancode())
					{
						viskbd.set(key.getUsbHidScancode(), key.value);
					}
					else if (key.scancode == 0x409)
					{
						viskbd.values[visKeyboard::KEY_FN] = key.value;
					}
				}
				w.redraw();
			}
			return;
		}
#endif
		// Digital keyboard
		uint8_t dblbuf[visKeyboard::NUM_KEYS];
		memset(dblbuf, 0, sizeof(dblbuf));
		while (running)
		{
			dblbuf[visKeyboard::KEY_BACKQUOTE] = (GetAsyncKeyState(VK_OEM_3) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_1] = (GetAsyncKeyState('1') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_2] = (GetAsyncKeyState('2') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_3] = (GetAsyncKeyState('3') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_4] = (GetAsyncKeyState('4') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_5] = (GetAsyncKeyState('5') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_6] = (GetAsyncKeyState('6') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_7] = (GetAsyncKeyState('7') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_8] = (GetAsyncKeyState('8') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_9] = (GetAsyncKeyState('9') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_0] = (GetAsyncKeyState('0') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_MINUS] = (GetAsyncKeyState(VK_OEM_MINUS) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_EQUALS] = (GetAsyncKeyState(VK_OEM_PLUS) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_BACKSPACE] = (GetAsyncKeyState(VK_BACK) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_TAB] = (GetAsyncKeyState(VK_TAB) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_Q] = (GetAsyncKeyState('Q') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_W] = (GetAsyncKeyState('W') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_E] = (GetAsyncKeyState('E') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_R] = (GetAsyncKeyState('R') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_T] = (GetAsyncKeyState('T') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_Y] = (GetAsyncKeyState('Y') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_U] = (GetAsyncKeyState('U') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_I] = (GetAsyncKeyState('I') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_O] = (GetAsyncKeyState('O') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_P] = (GetAsyncKeyState('P') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_BRACKET_LEFT] = (GetAsyncKeyState(VK_OEM_4) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_BRACKET_RIGHT] = (GetAsyncKeyState(VK_OEM_6) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_CAPSLOCK] = (GetAsyncKeyState(VK_CAPITAL) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_A] = (GetAsyncKeyState('A') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_S] = (GetAsyncKeyState('S') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_D] = (GetAsyncKeyState('D') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_F] = (GetAsyncKeyState('F') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_G] = (GetAsyncKeyState('G') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_H] = (GetAsyncKeyState('H') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_J] = (GetAsyncKeyState('J') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_K] = (GetAsyncKeyState('K') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_L] = (GetAsyncKeyState('L') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_SEMICOLON] = (GetAsyncKeyState(VK_OEM_1) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_QUOTE] = (GetAsyncKeyState(VK_OEM_7) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_BACKSLASH] = (GetAsyncKeyState(VK_OEM_5) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_ENTER] = (GetAsyncKeyState(VK_RETURN) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_LSHIFT] = (GetAsyncKeyState(VK_LSHIFT) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_Z] = (GetAsyncKeyState('Z') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_X] = (GetAsyncKeyState('X') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_C] = (GetAsyncKeyState('C') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_V] = (GetAsyncKeyState('V') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_B] = (GetAsyncKeyState('B') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_N] = (GetAsyncKeyState('N') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_M] = (GetAsyncKeyState('M') & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_COMMA] = (GetAsyncKeyState(VK_OEM_COMMA) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_PERIOD] = (GetAsyncKeyState(VK_OEM_PERIOD) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_SLASH] = (GetAsyncKeyState(VK_OEM_2) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_RSHIFT] = (GetAsyncKeyState(VK_RSHIFT) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_LCTRL] = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_LMETA] = (GetAsyncKeyState(VK_LWIN) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_LALT] = (GetAsyncKeyState(VK_LMENU) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_SPACE] = (GetAsyncKeyState(VK_SPACE) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_RALT] = (GetAsyncKeyState(VK_RMENU) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_RMETA] = (GetAsyncKeyState(VK_RWIN) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_RCTRL] = (GetAsyncKeyState(VK_RCONTROL) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_NUMLOCK] = (GetAsyncKeyState(VK_NUMLOCK) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_NUMPAD_DIVIDE] = (GetAsyncKeyState(VK_DIVIDE) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_NUMPAD_MULTIPLY] = (GetAsyncKeyState(VK_MULTIPLY) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_NUMPAD_SUBTRACT] = (GetAsyncKeyState(VK_SUBTRACT) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_NUMPAD7] = (GetAsyncKeyState(VK_NUMPAD7) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_NUMPAD8] = (GetAsyncKeyState(VK_NUMPAD8) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_NUMPAD9] = (GetAsyncKeyState(VK_NUMPAD9) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_NUMPAD_ADD] = (GetAsyncKeyState(VK_ADD) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_NUMPAD4] = (GetAsyncKeyState(VK_NUMPAD4) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_NUMPAD5] = (GetAsyncKeyState(VK_NUMPAD5) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_NUMPAD6] = (GetAsyncKeyState(VK_NUMPAD6) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_NUMPAD1] = (GetAsyncKeyState(VK_NUMPAD1) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_NUMPAD2] = (GetAsyncKeyState(VK_NUMPAD2) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_NUMPAD3] = (GetAsyncKeyState(VK_NUMPAD3) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_NUMPAD_ENTER] = (GetAsyncKeyState(VK_RETURN) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_NUMPAD0] = (GetAsyncKeyState(VK_NUMPAD0) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_NUMPAD_DECIMAL] = (GetAsyncKeyState(VK_DECIMAL) & 0x8000) ? 255 : 0;
			if (memcmp(viskbd.values, dblbuf, sizeof(dblbuf)) != 0)
			{
				memcpy(viskbd.values, dblbuf, sizeof(dblbuf));
				w.redraw();
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	});

	w = Window::create("Soup", 195 * 5, (3 * 8) + 18 + 50 * 5);
	w.setDrawFunc([](Window, RenderTarget& rt)
	{
		rt.fill(Rgb::GREEN);
		rt.drawText(9, 9, kbd_name, RasterFont::simple8(), Rgb::WHITE, 3);
		viskbd.draw(rt, 0, (3 * 8) + 18, 5);
	});
	w.setInvisibleColour(Rgb::GREEN);
	w.setTopmost(true);
	w.runMessageLoop();

	running = false;
	CancelSynchronousIo(t.handle);
	t.awaitCompletion();
#else
	std::cout << "Sorry, this only works on Windows (for now).\n";
#endif
}
