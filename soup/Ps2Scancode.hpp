#pragma once

#include <cstdint>

#include "base.hpp"

NAMESPACE_SOUP
{
	/*
	* My PCs haven't had a PS/2 port in a few generations, but... Windows. :)
	* 
	* Many values from https://stackoverflow.com/a/69600455
	*/
	enum Ps2Scancode : uint16_t
	{
		PS2_A = 0x001E,
		PS2_B = 0x0030,
		PS2_C = 0x002E,
		PS2_D = 0x0020,
		PS2_E = 0x0012,
		PS2_F = 0x0021,
		PS2_G = 0x0022,
		PS2_H = 0x0023,
		PS2_I = 0x0017,
		PS2_J = 0x0024,
		PS2_K = 0x0025,
		PS2_L = 0x0026,
		PS2_M = 0x0032,
		PS2_N = 0x0031,
		PS2_O = 0x0018,
		PS2_P = 0x0019,
		PS2_Q = 0x0010,
		PS2_R = 0x0013,
		PS2_S = 0x001F,
		PS2_T = 0x0014,
		PS2_U = 0x0016,
		PS2_V = 0x002F,
		PS2_W = 0x0011,
		PS2_X = 0x002D,
		PS2_Y = 0x0015,
		PS2_Z = 0x002C,
		PS2_1 = 0x0002,
		PS2_2 = 0x0003,
		PS2_3 = 0x0004,
		PS2_4 = 0x0005,
		PS2_5 = 0x0006,
		PS2_6 = 0x0007,
		PS2_7 = 0x0008,
		PS2_8 = 0x0009,
		PS2_9 = 0x000A,
		PS2_0 = 0x000B,
		PS2_ENTER = 0x001C,
		PS2_ESCAPE = 0x0001,
		PS2_BACKSPACE = 0x000E,
		PS2_TAB = 0x000F,
		PS2_SPACE = 0x0039,
		PS2_MINUS = 0x000C,
		PS2_EQUALS = 0x000D,
		PS2_BRACKET_LEFT = 0x001A,
		PS2_BRACKET_RIGHT = 0x001B,
		PS2_BACKSLASH = 0x002B,
		PS2_SEMICOLON = 0x0027,
		PS2_QUOTE = 0x0028,
		PS2_BACKQUOTE = 0x0029,
		PS2_COMMA = 0x0033,
		PS2_PERIOD = 0x0034,
		PS2_SLASH = 0x0035,
		PS2_CAPS_LOCK = 0x003A,
		PS2_F1 = 0x003B,
		PS2_F2 = 0x003C,
		PS2_F3 = 0x003D,
		PS2_F4 = 0x003E,
		PS2_F5 = 0x003F,
		PS2_F6 = 0x0040,
		PS2_F7 = 0x0041,
		PS2_F8 = 0x0042,
		PS2_F9 = 0x0043,
		PS2_F10 = 0x0044,
		PS2_F11 = 0x0057,
		PS2_F12 = 0x0058,
		PS2_INTL_BACKSLASH = 0x0056,
		PS2_LCTRL = 0x001D,
		PS2_LSHIFT = 0x002A,
		PS2_LALT = 0x0038,
		PS2_LMETA = 0xE05B,
		PS2_RCTRL = 0xE01D,
		PS2_RSHIFT = 0x0036,
		PS2_RALT = 0xE038,
		PS2_RMETA = 0xE05C,

		PS2_PRINT_SCREEN = 0xE037,
		PS2_SCROLL_LOCK = 0x0046,

		PS2_INSERT = 0xE052,
		PS2_HOME = 0xE047,
		PS2_PAGE_UP = 0xE049,
		PS2_DEL = 0xE053,
		PS2_END = 0xE04F,
		PS2_PAGE_DOWN = 0xE051,

		PS2_ARROW_UP = 0xE048,
		PS2_ARROW_LEFT = 0xE04B,
		PS2_ARROW_DOWN = 0xE050,
		PS2_ARROW_RIGHT = 0xE04D,

		PS2_NUM_LOCK = 0x0045,
		PS2_NUMPAD_DIVIDE = 0xE035,
		PS2_NUMPAD_MULTIPLY = 0x0037,
		PS2_NUMPAD_SUBTRACT = 0x004A,
		PS2_NUMPAD7 = 0x0047,
		PS2_NUMPAD8 = 0x0048,
		PS2_NUMPAD9 = 0x0049,
		PS2_NUMPAD_ADD = 0x004E,
		PS2_NUMPAD4 = 0x004B,
		PS2_NUMPAD5 = 0x004C,
		PS2_NUMPAD6 = 0x004D,
		PS2_NUMPAD1 = 0x004F,
		PS2_NUMPAD2 = 0x0050,
		PS2_NUMPAD3 = 0x0051,
		PS2_NUMPAD_ENTER = 0xE01C,
		PS2_NUMPAD0 = 0x0052,
		PS2_NUMPAD_DECIMAL = 0x0053,

		PS2_F13 = 0x005D,
		PS2_F14 = 0x005E,
		PS2_F15 = 0x005F,

		// The following keys were obtained via MapVirtualKeyA(..., MAPVK_VK_TO_VSC_EX)
		// However, I'm not sure how universal they are.

		PS2_PAUSE = 0xE11D,

		PS2_STOP_MEDIA = 0xE024,
		PS2_PREV_TRACK = 0xE010,
		PS2_PLAY_PAUSE = 0xE022,
		PS2_NEXT_TRACK = 0xE019,
	};
}
