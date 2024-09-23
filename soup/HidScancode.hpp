#pragma once

#include <cstdint>

#include "Key.hpp"

NAMESPACE_SOUP
{
	/*
	* Based on https://github.com/WootingKb/wooting-analog-sdk/blob/develop/wooting-analog-sdk/src/keycode.rs
	* This also looks good: https://gist.github.com/MightyPork/6da26e382a7ad91b5496ee55fdc73db2
	*
	* Instead of "scancodes" they might be called "Usage IDs" as on page 53 of https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
	*/
	enum HidScancode : uint8_t
	{
		HID_A = 0x04,
		HID_B = 0x05,
		HID_C = 0x06,
		HID_D = 0x07,
		HID_E = 0x08,
		HID_F = 0x09,
		HID_G = 0x0a,
		HID_H = 0x0b,
		HID_I = 0x0c,
		HID_J = 0x0d,
		HID_K = 0x0e,
		HID_L = 0x0f,
		HID_M = 0x10,
		HID_N = 0x11,
		HID_O = 0x12,
		HID_P = 0x13,
		HID_Q = 0x14,
		HID_R = 0x15,
		HID_S = 0x16,
		HID_T = 0x17,
		HID_U = 0x18,
		HID_V = 0x19,
		HID_W = 0x1a,
		HID_X = 0x1b,
		HID_Y = 0x1c,
		HID_Z = 0x1d,
		HID_1 = 0x1e,
		HID_2 = 0x1f,
		HID_3 = 0x20,
		HID_4 = 0x21,
		HID_5 = 0x22,
		HID_6 = 0x23,
		HID_7 = 0x24,
		HID_8 = 0x25,
		HID_9 = 0x26,
		HID_0 = 0x27,
		HID_ENTER = 0x28,
		HID_ESCAPE = 0x29,
		HID_BACKSPACE = 0x2a,
		HID_TAB = 0x2b,
		HID_SPACE = 0x2c,
		HID_MINUS = 0x2d,
		HID_EQUALS = 0x2e,
		HID_BRACKET_LEFT = 0x2f,
		HID_BRACKET_RIGHT = 0x30,
		HID_BACKSLASH = 0x31,
		HID_INTL_HASH = 0x32,
		HID_SEMICOLON = 0x33,
		HID_QUOTE = 0x34,
		HID_BACKQUOTE = 0x35,
		HID_COMMA = 0x36,
		HID_PERIOD = 0x37,
		HID_SLASH = 0x38,
		HID_CAPS_LOCK = 0x39,
		HID_F1 = 0x3a,
		HID_F2 = 0x3b,
		HID_F3 = 0x3c,
		HID_F4 = 0x3d,
		HID_F5 = 0x3e,
		HID_F6 = 0x3f,
		HID_F7 = 0x40,
		HID_F8 = 0x41,
		HID_F9 = 0x42,
		HID_F10 = 0x43,
		HID_F11 = 0x44,
		HID_F12 = 0x45,
		HID_PRINT_SCREEN = 0x46,
		HID_SCROLL_LOCK = 0x47,
		HID_PAUSE = 0x48,
		HID_INSERT = 0x49,
		HID_HOME = 0x4a,
		HID_PAGE_UP = 0x4b,
		HID_DEL = 0x4c,
		HID_END = 0x4d,
		HID_PAGE_DOWN = 0x4e,
		HID_ARROW_RIGHT = 0x4f,
		HID_ARROW_LEFT = 0x50,
		HID_ARROW_DOWN = 0x51,
		HID_ARROW_UP = 0x52,
		HID_NUM_LOCK = 0x53,
		HID_NUMPAD_DIVIDE = 0x54,
		HID_NUMPAD_MULTIPLY = 0x55,
		HID_NUMPAD_SUBTRACT = 0x56,
		HID_NUMPAD_ADD = 0x57,
		HID_NUMPAD_ENTER = 0x58,
		HID_NUMPAD1 = 0x59,
		HID_NUMPAD2 = 0x5a,
		HID_NUMPAD3 = 0x5b,
		HID_NUMPAD4 = 0x5c,
		HID_NUMPAD5 = 0x5d,
		HID_NUMPAD6 = 0x5e,
		HID_NUMPAD7 = 0x5f,
		HID_NUMPAD8 = 0x60,
		HID_NUMPAD9 = 0x61,
		HID_NUMPAD0 = 0x62,
		HID_NUMPAD_DECIMAL = 0x63,
		HID_INTL_BACKSLASH = 0x64,
		HID_CONTEXT_MENU = 0x65,
		HID_POWER = 0x66,
		HID_NUMPAD_EQUAL = 0x67,
		HID_F13 = 0x68,
		HID_F14 = 0x69,
		HID_F15 = 0x6a,
		HID_F16 = 0x6b,
		HID_F17 = 0x6c,
		HID_F18 = 0x6d,
		HID_F19 = 0x6e,
		HID_F20 = 0x6f,
		HID_F21 = 0x70,
		HID_F22 = 0x71,
		HID_F23 = 0x72,
		HID_F24 = 0x73,
		HID_OPEN = 0x74,
		HID_HELP = 0x75,
		HID_SELECT = 0x77,
		HID_AGAIN = 0x79,
		HID_UNDO = 0x7a,
		HID_CUT = 0x7b,
		HID_COPY = 0x7c,
		HID_PASTE = 0x7d,
		HID_FIND = 0x7e,
		HID_VOLUME_MUTE = 0x7f,
		HID_VOLUME_UP = 0x80,
		HID_VOLUME_DOWN = 0x81,
		HID_NUMPAD_COMMA = 0x85,
		HID_INTL_RO = 0x87,
		HID_KANA_MODE = 0x88,
		HID_INTL_YEN = 0x89,
		HID_CONVERT = 0x8a,
		HID_NON_CONVERT = 0x8b,
		HID_LANG1 = 0x90,
		HID_LANG2 = 0x91,
		HID_LANG3 = 0x92,
		HID_LANG4 = 0x93,
		HID_CONTROL_LEFT = 0xe0,
		HID_SHIFT_LEFT = 0xe1,
		HID_ALT_LEFT = 0xe2,
		HID_META_LEFT = 0xe3,
		HID_CONTROL_RIGHT = 0xe4,
		HID_SHIFT_RIGHT = 0xe5,
		HID_ALT_RIGHT = 0xe6,
		HID_META_RIGHT = 0xe7,
	};

	[[nodiscard]] constexpr Key hid_scancode_to_soup_key(uint8_t scancode) noexcept
	{
		switch (scancode)
		{
		case HID_ESCAPE: return KEY_ESCAPE;
		case HID_F1: return KEY_F1;
		case HID_F2: return KEY_F2;
		case HID_F3: return KEY_F3;
		case HID_F4: return KEY_F4;
		case HID_F5: return KEY_F5;
		case HID_F6: return KEY_F6;
		case HID_F7: return KEY_F7;
		case HID_F8: return KEY_F8;
		case HID_F9: return KEY_F9;
		case HID_F10: return KEY_F10;
		case HID_F11: return KEY_F11;
		case HID_F12: return KEY_F12;
		case HID_BACKQUOTE: return KEY_BACKQUOTE;

		case HID_1: return KEY_1;
		case HID_2: return KEY_2;
		case HID_3: return KEY_3;
		case HID_4: return KEY_4;
		case HID_5: return KEY_5;
		case HID_6: return KEY_6;
		case HID_7: return KEY_7;
		case HID_8: return KEY_8;
		case HID_9: return KEY_9;
		case HID_0: return KEY_0;
		case HID_MINUS: return KEY_MINUS;
		case HID_EQUALS: return KEY_EQUALS;
		case HID_BACKSPACE: return KEY_BACKSPACE;
		case HID_TAB: return KEY_TAB;
		case HID_Q: return KEY_Q;
		case HID_W: return KEY_W;
		case HID_E: return KEY_E;
		case HID_R: return KEY_R;
		case HID_T: return KEY_T;
		case HID_Y: return KEY_Y;
		case HID_U: return KEY_U;
		case HID_I: return KEY_I;
		case HID_O: return KEY_O;
		case HID_P: return KEY_P;
		case HID_BRACKET_LEFT: return KEY_BRACKET_LEFT;
		case HID_BRACKET_RIGHT: return KEY_BRACKET_RIGHT;
		case HID_ENTER: return KEY_ENTER;
		case HID_CAPS_LOCK: return KEY_CAPS_LOCK;
		case HID_A: return KEY_A;
		case HID_S: return KEY_S;
		case HID_D: return KEY_D;
		case HID_F: return KEY_F;
		case HID_G: return KEY_G;
		case HID_H: return KEY_H;
		case HID_J: return KEY_J;
		case HID_K: return KEY_K;
		case HID_L: return KEY_L;
		case HID_SEMICOLON: return KEY_SEMICOLON;
		case HID_QUOTE: return KEY_QUOTE;
		case HID_BACKSLASH: return KEY_BACKSLASH;
		case HID_SHIFT_LEFT: return KEY_LSHIFT;
		case HID_INTL_BACKSLASH: return KEY_INTL_BACKSLASH;
		case HID_Z: return KEY_Z;
		case HID_X: return KEY_X;
		case HID_C: return KEY_C;
		case HID_V: return KEY_V;
		case HID_B: return KEY_B;
		case HID_N: return KEY_N;
		case HID_M: return KEY_M;
		case HID_COMMA: return KEY_COMMA;
		case HID_PERIOD: return KEY_PERIOD;
		case HID_SLASH: return KEY_SLASH;
		case HID_SHIFT_RIGHT: return KEY_RSHIFT;
		case HID_CONTROL_LEFT: return KEY_LCTRL;
		case HID_META_LEFT: return KEY_LMETA;
		case HID_ALT_LEFT: return KEY_LALT;
		case HID_LANG2: return KEY_IME_OFF;
		case HID_SPACE: return KEY_SPACE;
		case HID_LANG1: return KEY_IME_ON;
		case HID_ALT_RIGHT: return KEY_RALT;
		case HID_META_RIGHT: return KEY_RMETA;
			// no HID for KEY_FN
		case HID_CONTROL_RIGHT: return KEY_RCTRL;

		case HID_PRINT_SCREEN: return KEY_PRINT_SCREEN;
		case HID_PAUSE: return KEY_PAUSE;
		case HID_SCROLL_LOCK: return KEY_SCROLL_LOCK;

		case HID_INSERT: return KEY_INSERT;
		case HID_HOME: return KEY_HOME;
		case HID_PAGE_UP: return KEY_PAGE_UP;
		case HID_DEL: return KEY_DEL;
		case HID_END: return KEY_END;
		case HID_PAGE_DOWN: return KEY_PAGE_DOWN;

		case HID_ARROW_UP: return KEY_ARROW_UP;
		case HID_ARROW_LEFT: return KEY_ARROW_LEFT;
		case HID_ARROW_DOWN: return KEY_ARROW_DOWN;
		case HID_ARROW_RIGHT: return KEY_ARROW_RIGHT;

		case HID_NUM_LOCK: return KEY_NUM_LOCK;
		case HID_NUMPAD_DIVIDE: return KEY_NUMPAD_DIVIDE;
		case HID_NUMPAD_MULTIPLY: return KEY_NUMPAD_MULTIPLY;
		case HID_NUMPAD_SUBTRACT: return KEY_NUMPAD_SUBTRACT;
		case HID_NUMPAD7: return KEY_NUMPAD7;
		case HID_NUMPAD8: return KEY_NUMPAD8;
		case HID_NUMPAD9: return KEY_NUMPAD9;
		case HID_NUMPAD_ADD: return KEY_NUMPAD_ADD;
		case HID_NUMPAD4: return KEY_NUMPAD4;
		case HID_NUMPAD5: return KEY_NUMPAD5;
		case HID_NUMPAD6: return KEY_NUMPAD6;
		case HID_NUMPAD1: return KEY_NUMPAD1;
		case HID_NUMPAD2: return KEY_NUMPAD2;
		case HID_NUMPAD3: return KEY_NUMPAD3;
		case HID_NUMPAD_ENTER: return KEY_NUMPAD_ENTER;
		case HID_NUMPAD0: return KEY_NUMPAD0;
		case HID_NUMPAD_DECIMAL: return KEY_NUMPAD_DECIMAL;

		case HID_F13: return KEY_F13;
		case HID_F14: return KEY_F14;
		case HID_F15: return KEY_F15;
		case HID_F16: return KEY_F16;
		case HID_F17: return KEY_F17;
		case HID_F18: return KEY_F18;
		case HID_F19: return KEY_F19;
		case HID_F20: return KEY_F20;
		case HID_F21: return KEY_F21;
		case HID_F22: return KEY_F22;
		}
		return KEY_NONE;
	}

	[[nodiscard]] constexpr uint8_t soup_key_to_hid_scancode(uint8_t scancode) noexcept
	{
		switch (scancode)
		{
		case KEY_ESCAPE: return HID_ESCAPE;
		case KEY_F1: return HID_F1;
		case KEY_F2: return HID_F2;
		case KEY_F3: return HID_F3;
		case KEY_F4: return HID_F4;
		case KEY_F5: return HID_F5;
		case KEY_F6: return HID_F6;
		case KEY_F7: return HID_F7;
		case KEY_F8: return HID_F8;
		case KEY_F9: return HID_F9;
		case KEY_F10: return HID_F10;
		case KEY_F11: return HID_F11;
		case KEY_F12: return HID_F12;
		case KEY_BACKQUOTE: return HID_BACKQUOTE;

		case KEY_1: return HID_1;
		case KEY_2: return HID_2;
		case KEY_3: return HID_3;
		case KEY_4: return HID_4;
		case KEY_5: return HID_5;
		case KEY_6: return HID_6;
		case KEY_7: return HID_7;
		case KEY_8: return HID_8;
		case KEY_9: return HID_9;
		case KEY_0: return HID_0;
		case KEY_MINUS: return HID_MINUS;
		case KEY_EQUALS: return HID_EQUALS;
		case KEY_BACKSPACE: return HID_BACKSPACE;
		case KEY_TAB: return HID_TAB;
		case KEY_Q: return HID_Q;
		case KEY_W: return HID_W;
		case KEY_E: return HID_E;
		case KEY_R: return HID_R;
		case KEY_T: return HID_T;
		case KEY_Y: return HID_Y;
		case KEY_U: return HID_U;
		case KEY_I: return HID_I;
		case KEY_O: return HID_O;
		case KEY_P: return HID_P;
		case KEY_BRACKET_LEFT: return HID_BRACKET_LEFT;
		case KEY_BRACKET_RIGHT: return HID_BRACKET_RIGHT;
		case KEY_ENTER: return HID_ENTER;
		case KEY_CAPS_LOCK: return HID_CAPS_LOCK;
		case KEY_A: return HID_A;
		case KEY_S: return HID_S;
		case KEY_D: return HID_D;
		case KEY_F: return HID_F;
		case KEY_G: return HID_G;
		case KEY_H: return HID_H;
		case KEY_J: return HID_J;
		case KEY_K: return HID_K;
		case KEY_L: return HID_L;
		case KEY_SEMICOLON: return HID_SEMICOLON;
		case KEY_QUOTE: return HID_QUOTE;
		case KEY_BACKSLASH: return HID_BACKSLASH;
		case KEY_LSHIFT: return HID_SHIFT_LEFT;
		case KEY_INTL_BACKSLASH: return HID_INTL_BACKSLASH;
		case KEY_Z: return HID_Z;
		case KEY_X: return HID_X;
		case KEY_C: return HID_C;
		case KEY_V: return HID_V;
		case KEY_B: return HID_B;
		case KEY_N: return HID_N;
		case KEY_M: return HID_M;
		case KEY_COMMA: return HID_COMMA;
		case KEY_PERIOD: return HID_PERIOD;
		case KEY_SLASH: return HID_SLASH;
		case KEY_RSHIFT: return HID_SHIFT_RIGHT;
		case KEY_LCTRL: return HID_CONTROL_LEFT;
		case KEY_LMETA: return HID_META_LEFT;
		case KEY_LALT: return HID_ALT_LEFT;
		case KEY_IME_OFF: return HID_LANG2;
		case KEY_SPACE: return HID_SPACE;
		case KEY_IME_ON: return HID_LANG1;
		case KEY_RALT: return HID_ALT_RIGHT;
		case KEY_RMETA: return HID_META_RIGHT;
			// no HID for KEY_FN
		case KEY_RCTRL: return HID_CONTROL_RIGHT;

		case KEY_PRINT_SCREEN: return HID_PRINT_SCREEN;
		case KEY_PAUSE: return HID_PAUSE;
		case KEY_SCROLL_LOCK: return HID_SCROLL_LOCK;

		case KEY_INSERT: return HID_INSERT;
		case KEY_HOME: return HID_HOME;
		case KEY_PAGE_UP: return HID_PAGE_UP;
		case KEY_DEL: return HID_DEL;
		case KEY_END: return HID_END;
		case KEY_PAGE_DOWN: return HID_PAGE_DOWN;

		case KEY_ARROW_UP: return HID_ARROW_UP;
		case KEY_ARROW_LEFT: return HID_ARROW_LEFT;
		case KEY_ARROW_DOWN: return HID_ARROW_DOWN;
		case KEY_ARROW_RIGHT: return HID_ARROW_RIGHT;

		case KEY_NUM_LOCK: return HID_NUM_LOCK;
		case KEY_NUMPAD_DIVIDE: return HID_NUMPAD_DIVIDE;
		case KEY_NUMPAD_MULTIPLY: return HID_NUMPAD_MULTIPLY;
		case KEY_NUMPAD_SUBTRACT: return HID_NUMPAD_SUBTRACT;
		case KEY_NUMPAD7: return HID_NUMPAD7;
		case KEY_NUMPAD8: return HID_NUMPAD8;
		case KEY_NUMPAD9: return HID_NUMPAD9;
		case KEY_NUMPAD_ADD: return HID_NUMPAD_ADD;
		case KEY_NUMPAD4: return HID_NUMPAD4;
		case KEY_NUMPAD5: return HID_NUMPAD5;
		case KEY_NUMPAD6: return HID_NUMPAD6;
		case KEY_NUMPAD1: return HID_NUMPAD1;
		case KEY_NUMPAD2: return HID_NUMPAD2;
		case KEY_NUMPAD3: return HID_NUMPAD3;
		case KEY_NUMPAD_ENTER: return HID_NUMPAD_ENTER;
		case KEY_NUMPAD0: return HID_NUMPAD0;
		case KEY_NUMPAD_DECIMAL: return HID_NUMPAD_DECIMAL;

		case KEY_F13: return HID_F13;
		case KEY_F14: return HID_F14;
		case KEY_F15: return HID_F15;
		case KEY_F16: return HID_F16;
		case KEY_F17: return HID_F17;
		case KEY_F18: return HID_F18;
		case KEY_F19: return HID_F19;
		case KEY_F20: return HID_F20;
		case KEY_F21: return HID_F21;
		case KEY_F22: return HID_F22;
		}
		return 0;
	}
}
