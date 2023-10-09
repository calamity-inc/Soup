#include "keys.hpp"

#include "Ps2Scancode.hpp"

#if SOUP_WINDOWS
#include "Window.hpp"
#endif

namespace soup
{
#if SOUP_WINDOWS
	[[nodiscard]] static int soup_key_to_virtual_key_common(uint8_t key)
	{
		// MapVirtualKeyA would return VK_INSERT instead of VK_NUMPAD0 etc.
		switch (key)
		{
		case KEY_NUMPAD0: return VK_NUMPAD0;
		case KEY_NUMPAD1: return VK_NUMPAD1;
		case KEY_NUMPAD2: return VK_NUMPAD2;
		case KEY_NUMPAD3: return VK_NUMPAD3;
		case KEY_NUMPAD4: return VK_NUMPAD4;
		case KEY_NUMPAD5: return VK_NUMPAD5;
		case KEY_NUMPAD6: return VK_NUMPAD6;
		case KEY_NUMPAD7: return VK_NUMPAD7;
		case KEY_NUMPAD8: return VK_NUMPAD8;
		case KEY_NUMPAD9: return VK_NUMPAD9;
		case KEY_NUMPAD_DECIMAL: return VK_DECIMAL;
		default:;
		}
		return 0;
	}

	int soup_key_to_virtual_key(uint8_t key) noexcept
	{
		if (int vk = soup_key_to_virtual_key_common(key))
		{
			return vk;
		}
		return MapVirtualKeyA(soup_key_to_ps2_scancode(key), MAPVK_VSC_TO_VK_EX);
	}

	int soup_key_to_translated_virtual_key(uint8_t key) noexcept
	{
		if (int vk = soup_key_to_virtual_key_common(key))
		{
			return vk;
		}
		const auto layout = GetKeyboardLayout(Window::getFocused().getOwnerTid());
		return MapVirtualKeyExA(soup_key_to_ps2_scancode(key), MAPVK_VSC_TO_VK_EX, layout);
	}
#endif

	uint16_t soup_key_to_ps2_scancode(uint8_t key) noexcept
	{
		switch (key)
		{
		case KEY_ESCAPE: return PS2_ESCAPE;
		case KEY_F1: return PS2_F1;
		case KEY_F2: return PS2_F2;
		case KEY_F3: return PS2_F3;
		case KEY_F4: return PS2_F4;
		case KEY_F5: return PS2_F5;
		case KEY_F6: return PS2_F6;
		case KEY_F7: return PS2_F7;
		case KEY_F8: return PS2_F8;
		case KEY_F9: return PS2_F9;
		case KEY_F10: return PS2_F10;
		case KEY_F11: return PS2_F11;
		case KEY_F12: return PS2_F12;

		case KEY_BACKQUOTE: return PS2_BACKQUOTE;
		case KEY_1: return PS2_1;
		case KEY_2: return PS2_2;
		case KEY_3: return PS2_3;
		case KEY_4: return PS2_4;
		case KEY_5: return PS2_5;
		case KEY_6: return PS2_6;
		case KEY_7: return PS2_7;
		case KEY_8: return PS2_8;
		case KEY_9: return PS2_9;
		case KEY_0: return PS2_0;
		case KEY_MINUS: return PS2_MINUS;
		case KEY_EQUALS: return PS2_EQUALS;
		case KEY_BACKSPACE: return PS2_BACKSPACE;
		case KEY_TAB: return PS2_TAB;
		case KEY_Q: return PS2_Q;
		case KEY_W: return PS2_W;
		case KEY_E: return PS2_E;
		case KEY_R: return PS2_R;
		case KEY_T: return PS2_T;
		case KEY_Y: return PS2_Y;
		case KEY_U: return PS2_U;
		case KEY_I: return PS2_I;
		case KEY_O: return PS2_O;
		case KEY_P: return PS2_P;
		case KEY_BRACKET_LEFT: return PS2_BRACKET_LEFT;
		case KEY_BRACKET_RIGHT: return PS2_BRACKET_RIGHT;
		case KEY_ENTER: return PS2_ENTER;
		case KEY_CAPS_LOCK: return PS2_CAPS_LOCK;
		case KEY_A: return PS2_A;
		case KEY_S: return PS2_S;
		case KEY_D: return PS2_D;
		case KEY_F: return PS2_F;
		case KEY_G: return PS2_G;
		case KEY_H: return PS2_H;
		case KEY_J: return PS2_J;
		case KEY_K: return PS2_K;
		case KEY_L: return PS2_L;
		case KEY_SEMICOLON: return PS2_SEMICOLON;
		case KEY_QUOTE: return PS2_QUOTE;
		case KEY_BACKSLASH: return PS2_BACKSLASH;
		case KEY_LSHIFT: return PS2_LSHIFT;
		case KEY_INTL_BACKSLASH: return PS2_INTL_BACKSLASH;
		case KEY_Z: return PS2_Z;
		case KEY_X: return PS2_X;
		case KEY_C: return PS2_C;
		case KEY_V: return PS2_V;
		case KEY_B: return PS2_B;
		case KEY_N: return PS2_N;
		case KEY_M: return PS2_M;
		case KEY_COMMA: return PS2_COMMA;
		case KEY_PERIOD: return PS2_PERIOD;
		case KEY_SLASH: return PS2_SLASH;
		case KEY_RSHIFT: return PS2_RSHIFT;
		case KEY_LCTRL: return PS2_LCTRL;
		case KEY_LMETA: return PS2_LMETA;
		case KEY_LALT: return PS2_LALT;
		case KEY_SPACE: return PS2_SPACE;
		case KEY_RALT: return PS2_RALT;
		case KEY_RMETA: return PS2_RMETA;
			//case KEY_FN: return PS2_FN;
		case KEY_RCTRL: return PS2_RCTRL;

		case KEY_PRINT_SCREEN: return PS2_PRINT_SCREEN;
		case KEY_PAUSE: return PS2_PAUSE;
		case KEY_SCROLL_LOCK: return PS2_SCROLL_LOCK;

		case KEY_INSERT: return PS2_INSERT;
		case KEY_HOME: return PS2_HOME;
		case KEY_PAGE_UP: return PS2_PAGE_UP;
		case KEY_DEL: return PS2_DEL;
		case KEY_END: return PS2_END;
		case KEY_PAGE_DOWN: return PS2_PAGE_DOWN;

		case KEY_ARROW_UP: return PS2_ARROW_UP;
		case KEY_ARROW_LEFT: return PS2_ARROW_LEFT;
		case KEY_ARROW_DOWN: return PS2_ARROW_DOWN;
		case KEY_ARROW_RIGHT: return PS2_ARROW_RIGHT;

		case KEY_NUM_LOCK: return PS2_NUM_LOCK;
		case KEY_NUMPAD_DIVIDE: return PS2_NUMPAD_DIVIDE;
		case KEY_NUMPAD_MULTIPLY: return PS2_NUMPAD_MULTIPLY;
		case KEY_NUMPAD_SUBTRACT: return PS2_NUMPAD_SUBTRACT;
		case KEY_NUMPAD7: return PS2_NUMPAD7;
		case KEY_NUMPAD8: return PS2_NUMPAD8;
		case KEY_NUMPAD9: return PS2_NUMPAD9;
		case KEY_NUMPAD_ADD: return PS2_NUMPAD_ADD;
		case KEY_NUMPAD4: return PS2_NUMPAD4;
		case KEY_NUMPAD5: return PS2_NUMPAD5;
		case KEY_NUMPAD6: return PS2_NUMPAD6;
		case KEY_NUMPAD1: return PS2_NUMPAD1;
		case KEY_NUMPAD2: return PS2_NUMPAD2;
		case KEY_NUMPAD3: return PS2_NUMPAD3;
		case KEY_NUMPAD_ENTER: return PS2_NUMPAD_ENTER;
		case KEY_NUMPAD0: return PS2_NUMPAD0;
		case KEY_NUMPAD_DECIMAL: return PS2_NUMPAD_DECIMAL;
		}
		return 0;
	}
}
