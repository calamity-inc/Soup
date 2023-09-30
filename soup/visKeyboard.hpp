#pragma once

#include <cstring> // memset

#include "fwd.hpp"
#include "scancodes.hpp"

namespace soup
{
	class visKeyboard
	{
	public:
		enum Keys : uint8_t
		{
			KEY_BACKQUOTE = 0,
			KEY_1,
			KEY_2,
			KEY_3,
			KEY_4,
			KEY_5,
			KEY_6,
			KEY_7,
			KEY_8,
			KEY_9,
			KEY_0,
			KEY_MINUS,
			KEY_EQUALS,
			KEY_BACKSPACE,
			KEY_TAB,
			KEY_Q,
			KEY_W,
			KEY_E,
			KEY_R,
			KEY_T,
			KEY_Y,
			KEY_U,
			KEY_I,
			KEY_O,
			KEY_P,
			KEY_BRACKET_LEFT,
			KEY_BRACKET_RIGHT,
			KEY_ENTER,
			KEY_CAPS_LOCK,
			KEY_A,
			KEY_S,
			KEY_D,
			KEY_F,
			KEY_G,
			KEY_H,
			KEY_J,
			KEY_K,
			KEY_L,
			KEY_SEMICOLON,
			KEY_QUOTE,
			KEY_BACKSLASH,
			KEY_LSHIFT,
			KEY_INTL_BACKSLASH,
			KEY_Z,
			KEY_X,
			KEY_C,
			KEY_V,
			KEY_B,
			KEY_N,
			KEY_M,
			KEY_COMMA,
			KEY_PERIOD,
			KEY_SLASH,
			KEY_RSHIFT,
			KEY_LCTRL,
			KEY_LMETA,
			KEY_LALT,
			KEY_SPACE,
			KEY_RALT,
			KEY_RMETA,
			KEY_FN,
			KEY_RCTRL,

			KEY_NUM_LOCK,
			KEY_NUMPAD_DIVIDE,
			KEY_NUMPAD_MULTIPLY,
			KEY_NUMPAD_SUBTRACT,
			KEY_NUMPAD7,
			KEY_NUMPAD8,
			KEY_NUMPAD9,
			KEY_NUMPAD_ADD,
			KEY_NUMPAD4,
			KEY_NUMPAD5,
			KEY_NUMPAD6,
			KEY_NUMPAD1,
			KEY_NUMPAD2,
			KEY_NUMPAD3,
			KEY_NUMPAD_ENTER,
			KEY_NUMPAD0,
			KEY_NUMPAD_DECIMAL,

			NUM_KEYS
		};

		uint8_t values[NUM_KEYS];

		visKeyboard() noexcept
		{
			clear();
		}

		void clear() noexcept
		{
			memset(values, 0, sizeof(values));
		}

		void set(UsbHidScancode scancode, uint8_t value) noexcept
		{
			switch (scancode)
			{
			case HID_BACKQUOTE: values[KEY_BACKQUOTE] = value; break;
			case HID_1: values[KEY_1] = value; break;
			case HID_2: values[KEY_2] = value; break;
			case HID_3: values[KEY_3] = value; break;
			case HID_4: values[KEY_4] = value; break;
			case HID_5: values[KEY_5] = value; break;
			case HID_6: values[KEY_6] = value; break;
			case HID_7: values[KEY_7] = value; break;
			case HID_8: values[KEY_8] = value; break;
			case HID_9: values[KEY_9] = value; break;
			case HID_0: values[KEY_0] = value; break;
			case HID_MINUS: values[KEY_MINUS] = value; break;
			case HID_EQUALS: values[KEY_EQUALS] = value; break;
			case HID_BACKSPACE: values[KEY_BACKSPACE] = value; break;
			case HID_TAB: values[KEY_TAB] = value; break;
			case HID_Q: values[KEY_Q] = value; break;
			case HID_W: values[KEY_W] = value; break;
			case HID_E: values[KEY_E] = value; break;
			case HID_R: values[KEY_R] = value; break;
			case HID_T: values[KEY_T] = value; break;
			case HID_Y: values[KEY_Y] = value; break;
			case HID_U: values[KEY_U] = value; break;
			case HID_I: values[KEY_I] = value; break;
			case HID_O: values[KEY_O] = value; break;
			case HID_P: values[KEY_P] = value; break;
			case HID_BRACKET_LEFT: values[KEY_BRACKET_LEFT] = value; break;
			case HID_BRACKET_RIGHT: values[KEY_BRACKET_RIGHT] = value; break;
			case HID_ENTER: values[KEY_ENTER] = value; break;
			case HID_CAPS_LOCK: values[KEY_CAPS_LOCK] = value; break;
			case HID_A: values[KEY_A] = value; break;
			case HID_S: values[KEY_S] = value; break;
			case HID_D: values[KEY_D] = value; break;
			case HID_F: values[KEY_F] = value; break;
			case HID_G: values[KEY_G] = value; break;
			case HID_H: values[KEY_H] = value; break;
			case HID_J: values[KEY_J] = value; break;
			case HID_K: values[KEY_K] = value; break;
			case HID_L: values[KEY_L] = value; break;
			case HID_SEMICOLON: values[KEY_SEMICOLON] = value; break;
			case HID_QUOTE: values[KEY_QUOTE] = value; break;
			case HID_BACKSLASH: values[KEY_BACKSLASH] = value; break;
			case HID_SHIFT_LEFT: values[KEY_LSHIFT] = value; break;
			case HID_INTL_BACKSLASH: values[KEY_INTL_BACKSLASH] = value; break;
			case HID_Z: values[KEY_Z] = value; break;
			case HID_X: values[KEY_X] = value; break;
			case HID_C: values[KEY_C] = value; break;
			case HID_V: values[KEY_V] = value; break;
			case HID_B: values[KEY_B] = value; break;
			case HID_N: values[KEY_N] = value; break;
			case HID_M: values[KEY_M] = value; break;
			case HID_COMMA: values[KEY_COMMA] = value; break;
			case HID_PERIOD: values[KEY_PERIOD] = value; break;
			case HID_SLASH: values[KEY_SLASH] = value; break;
			case HID_SHIFT_RIGHT: values[KEY_RSHIFT] = value; break;
			case HID_CONTROL_LEFT: values[KEY_LCTRL] = value; break;
			case HID_META_LEFT: values[KEY_LMETA] = value; break;
			case HID_ALT_LEFT: values[KEY_LALT] = value; break;
			case HID_SPACE: values[KEY_SPACE] = value; break;
			case HID_ALT_RIGHT: values[KEY_RALT] = value; break;
			case HID_META_RIGHT: values[KEY_RMETA] = value; break;
				// no HID for KEY_FN
			case HID_CONTROL_RIGHT: values[KEY_RCTRL] = value; break;

			case HID_NUM_LOCK: values[KEY_NUM_LOCK] = value; break;
			case HID_NUMPAD_DIVIDE: values[KEY_NUMPAD_DIVIDE] = value; break;
			case HID_NUMPAD_MULTIPLY: values[KEY_NUMPAD_MULTIPLY] = value; break;
			case HID_NUMPAD_SUBTRACT: values[KEY_NUMPAD_SUBTRACT] = value; break;
			case HID_NUMPAD7: values[KEY_NUMPAD7] = value; break;
			case HID_NUMPAD8: values[KEY_NUMPAD8] = value; break;
			case HID_NUMPAD9: values[KEY_NUMPAD9] = value; break;
			case HID_NUMPAD_ADD: values[KEY_NUMPAD_ADD] = value; break;
			case HID_NUMPAD4: values[KEY_NUMPAD4] = value; break;
			case HID_NUMPAD5: values[KEY_NUMPAD5] = value; break;
			case HID_NUMPAD6: values[KEY_NUMPAD6] = value; break;
			case HID_NUMPAD1: values[KEY_NUMPAD1] = value; break;
			case HID_NUMPAD2: values[KEY_NUMPAD2] = value; break;
			case HID_NUMPAD3: values[KEY_NUMPAD3] = value; break;
			case HID_NUMPAD_ENTER: values[KEY_NUMPAD_ENTER] = value; break;
			case HID_NUMPAD0: values[KEY_NUMPAD0] = value; break;
			case HID_NUMPAD_DECIMAL: values[KEY_NUMPAD_DECIMAL] = value; break;

			default:; // silence warnings
			}
		}

		// 195 x 50 per scale
		void draw(RenderTarget& rt, unsigned int x, unsigned int y, uint8_t scale = 1) const;
	protected:
		void drawKey(RenderTarget& rt, uint8_t scale, const char* label, uint8_t value, unsigned int x, unsigned int y, unsigned int width = 10, unsigned int height = 10) const;
	};
}
