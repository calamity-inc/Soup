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

[[nodiscard]] static bool isPhysicalKeyPressed(uint16_t ps2_scancode) noexcept
{
	// GetKeyboardLayout(0) doesn't seem to react to layout changes without a program restart
	const auto layout = GetKeyboardLayout(Window::getFocused().getOwnerTid());
	const auto vk = MapVirtualKeyExA(ps2_scancode, MAPVK_VSC_TO_VK_EX, layout);
	return GetAsyncKeyState(vk) & 0x8000;
}
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
					//std::cout << std::hex << "Scancode " << (int)key.scancode << ", VK " << key.getVk() << ", VKT " << key.getVkTranslated() << "\n";
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
			dblbuf[visKeyboard::KEY_BACKQUOTE] = isPhysicalKeyPressed(PS2_BACKQUOTE) ? 255 : 0;
			dblbuf[visKeyboard::KEY_1] = isPhysicalKeyPressed(PS2_1) ? 255 : 0;
			dblbuf[visKeyboard::KEY_2] = isPhysicalKeyPressed(PS2_2) ? 255 : 0;
			dblbuf[visKeyboard::KEY_3] = isPhysicalKeyPressed(PS2_3) ? 255 : 0;
			dblbuf[visKeyboard::KEY_4] = isPhysicalKeyPressed(PS2_4) ? 255 : 0;
			dblbuf[visKeyboard::KEY_5] = isPhysicalKeyPressed(PS2_5) ? 255 : 0;
			dblbuf[visKeyboard::KEY_6] = isPhysicalKeyPressed(PS2_6) ? 255 : 0;
			dblbuf[visKeyboard::KEY_7] = isPhysicalKeyPressed(PS2_7) ? 255 : 0;
			dblbuf[visKeyboard::KEY_8] = isPhysicalKeyPressed(PS2_8) ? 255 : 0;
			dblbuf[visKeyboard::KEY_9] = isPhysicalKeyPressed(PS2_9) ? 255 : 0;
			dblbuf[visKeyboard::KEY_0] = isPhysicalKeyPressed(PS2_0) ? 255 : 0;
			dblbuf[visKeyboard::KEY_MINUS] = isPhysicalKeyPressed(PS2_MINUS) ? 255 : 0;
			dblbuf[visKeyboard::KEY_EQUALS] = isPhysicalKeyPressed(PS2_EQUALS) ? 255 : 0;
			dblbuf[visKeyboard::KEY_BACKSPACE] = isPhysicalKeyPressed(PS2_BACKSPACE) ? 255 : 0;
			dblbuf[visKeyboard::KEY_TAB] = isPhysicalKeyPressed(PS2_TAB) ? 255 : 0;
			dblbuf[visKeyboard::KEY_Q] = isPhysicalKeyPressed(PS2_Q) ? 255 : 0;
			dblbuf[visKeyboard::KEY_W] = isPhysicalKeyPressed(PS2_W) ? 255 : 0;
			dblbuf[visKeyboard::KEY_E] = isPhysicalKeyPressed(PS2_E) ? 255 : 0;
			dblbuf[visKeyboard::KEY_R] = isPhysicalKeyPressed(PS2_R) ? 255 : 0;
			dblbuf[visKeyboard::KEY_T] = isPhysicalKeyPressed(PS2_T) ? 255 : 0;
			dblbuf[visKeyboard::KEY_Y] = isPhysicalKeyPressed(PS2_Y) ? 255 : 0;
			dblbuf[visKeyboard::KEY_U] = isPhysicalKeyPressed(PS2_U) ? 255 : 0;
			dblbuf[visKeyboard::KEY_I] = isPhysicalKeyPressed(PS2_I) ? 255 : 0;
			dblbuf[visKeyboard::KEY_O] = isPhysicalKeyPressed(PS2_O) ? 255 : 0;
			dblbuf[visKeyboard::KEY_P] = isPhysicalKeyPressed(PS2_P) ? 255 : 0;
			dblbuf[visKeyboard::KEY_BRACKET_LEFT] = isPhysicalKeyPressed(PS2_BRACKET_LEFT) ? 255 : 0;
			dblbuf[visKeyboard::KEY_BRACKET_RIGHT] = isPhysicalKeyPressed(PS2_BRACKET_RIGHT) ? 255 : 0;
			dblbuf[visKeyboard::KEY_CAPS_LOCK] = isPhysicalKeyPressed(PS2_CAPS_LOCK) ? 255 : 0;
			dblbuf[visKeyboard::KEY_A] = isPhysicalKeyPressed(PS2_A) ? 255 : 0;
			dblbuf[visKeyboard::KEY_S] = isPhysicalKeyPressed(PS2_S) ? 255 : 0;
			dblbuf[visKeyboard::KEY_D] = isPhysicalKeyPressed(PS2_D) ? 255 : 0;
			dblbuf[visKeyboard::KEY_F] = isPhysicalKeyPressed(PS2_F) ? 255 : 0;
			dblbuf[visKeyboard::KEY_G] = isPhysicalKeyPressed(PS2_G) ? 255 : 0;
			dblbuf[visKeyboard::KEY_H] = isPhysicalKeyPressed(PS2_H) ? 255 : 0;
			dblbuf[visKeyboard::KEY_J] = isPhysicalKeyPressed(PS2_J) ? 255 : 0;
			dblbuf[visKeyboard::KEY_K] = isPhysicalKeyPressed(PS2_K) ? 255 : 0;
			dblbuf[visKeyboard::KEY_L] = isPhysicalKeyPressed(PS2_L) ? 255 : 0;
			dblbuf[visKeyboard::KEY_SEMICOLON] = isPhysicalKeyPressed(PS2_SEMICOLON) ? 255 : 0;
			dblbuf[visKeyboard::KEY_QUOTE] = isPhysicalKeyPressed(PS2_QUOTE) ? 255 : 0;
			dblbuf[visKeyboard::KEY_BACKSLASH] = isPhysicalKeyPressed(PS2_BACKSLASH) ? 255 : 0;
			dblbuf[visKeyboard::KEY_ENTER] = isPhysicalKeyPressed(PS2_ENTER) ? 255 : 0;
			dblbuf[visKeyboard::KEY_LSHIFT] = (GetAsyncKeyState(VK_LSHIFT) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_Z] = isPhysicalKeyPressed(PS2_Z) ? 255 : 0;
			dblbuf[visKeyboard::KEY_X] = isPhysicalKeyPressed(PS2_X) ? 255 : 0;
			dblbuf[visKeyboard::KEY_C] = isPhysicalKeyPressed(PS2_C) ? 255 : 0;
			dblbuf[visKeyboard::KEY_V] = isPhysicalKeyPressed(PS2_V) ? 255 : 0;
			dblbuf[visKeyboard::KEY_B] = isPhysicalKeyPressed(PS2_B) ? 255 : 0;
			dblbuf[visKeyboard::KEY_N] = isPhysicalKeyPressed(PS2_N) ? 255 : 0;
			dblbuf[visKeyboard::KEY_M] = isPhysicalKeyPressed(PS2_M) ? 255 : 0;
			dblbuf[visKeyboard::KEY_COMMA] = isPhysicalKeyPressed(PS2_COMMA) ? 255 : 0;
			dblbuf[visKeyboard::KEY_PERIOD] = isPhysicalKeyPressed(PS2_PERIOD) ? 255 : 0;
			dblbuf[visKeyboard::KEY_SLASH] = isPhysicalKeyPressed(PS2_SLASH) ? 255 : 0;
			dblbuf[visKeyboard::KEY_RSHIFT] = (GetAsyncKeyState(VK_RSHIFT) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_LCTRL] = (GetAsyncKeyState(VK_LCONTROL) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_LMETA] = (GetAsyncKeyState(VK_LWIN) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_LALT] = (GetAsyncKeyState(VK_LMENU) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_SPACE] = (GetAsyncKeyState(VK_SPACE) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_RALT] = (GetAsyncKeyState(VK_RMENU) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_RMETA] = (GetAsyncKeyState(VK_RWIN) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_RCTRL] = (GetAsyncKeyState(VK_RCONTROL) & 0x8000) ? 255 : 0;
			dblbuf[visKeyboard::KEY_NUM_LOCK] = (GetAsyncKeyState(VK_NUMLOCK) & 0x8000) ? 255 : 0;
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
			dblbuf[visKeyboard::KEY_NUMPAD_ENTER] = isPhysicalKeyPressed(0xE01C) ? 255 : 0;
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
