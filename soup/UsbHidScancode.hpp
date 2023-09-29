#pragma once

#include <cstdint>

namespace soup
{
	// Based on https://github.com/WootingKb/wooting-analog-sdk/blob/develop/wooting-analog-sdk/src/keycode.rs
	// This also looks good: https://gist.github.com/MightyPork/6da26e382a7ad91b5496ee55fdc73db2

	// Instead of "scancodes" they might be called "Usage IDs" as on page 53 of https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf
	enum UsbHidScancode : uint8_t
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
		HID_DIGIT1 = 0x1e,
		HID_DIGIT2 = 0x1f,
		HID_DIGIT3 = 0x20,
		HID_DIGIT4 = 0x21,
		HID_DIGIT5 = 0x22,
		HID_DIGIT6 = 0x23,
		HID_DIGIT7 = 0x24,
		HID_DIGIT8 = 0x25,
		HID_DIGIT9 = 0x26,
		HID_DIGIT0 = 0x27,
		HID_ENTER = 0x28,
		HID_ESCAPE = 0x29,
		HID_BACKSPACE = 0x2a,
		HID_TAB = 0x2b,
		HID_SPACE = 0x2c,
		HID_MINUS = 0x2d,
		HID_EQUAL = 0x2e,
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

	// My PCs haven't had a PS/2 port in a few generations, but... Windows. :)

	[[nodiscard]] constexpr uint16_t usb_hid_scancode_to_ps2_scancode(uint8_t scancode) noexcept
	{
		// https://stackoverflow.com/a/69600455
		switch (scancode)
		{
		case 0x01: return 0x00FF; // Overrun Error
		case 0x02: return 0x00FC; // POST Fail
		case 0x04: return 0x001E; // a A
		case 0x05: return 0x0030; // b B
		case 0x06: return 0x002E; // c C
		case 0x07: return 0x0020; // d D
		case 0x08: return 0x0012; // e E
		case 0x09: return 0x0021; // f F
		case 0x0A: return 0x0022; // g G
		case 0x0B: return 0x0023; // h H
		case 0x0C: return 0x0017; // i I
		case 0x0D: return 0x0024; // j J
		case 0x0E: return 0x0025; // k K
		case 0x0F: return 0x0026; // l L
		case 0x10: return 0x0032; // m M
		case 0x11: return 0x0031; // n N
		case 0x12: return 0x0018; // o O
		case 0x13: return 0x0019; // p P
		case 0x14: return 0x0010; // q Q
		case 0x15: return 0x0013; // r R
		case 0x16: return 0x001F; // s S
		case 0x17: return 0x0014; // t T
		case 0x18: return 0x0016; // u U
		case 0x19: return 0x002F; // v V
		case 0x1A: return 0x0011; // w W
		case 0x1B: return 0x002D; // x X
		case 0x1C: return 0x0015; // y Y
		case 0x1D: return 0x002C; // z Z
		case 0x1E: return 0x0002; // 1 !
		case 0x1F: return 0x0003; // 2 @
		case 0x20: return 0x0004; // 3 #
		case 0x21: return 0x0005; // 4 $
		case 0x22: return 0x0006; // 5 %
		case 0x23: return 0x0007; // 6 ^
		case 0x24: return 0x0008; // 7 &
		case 0x25: return 0x0009; // 8 *
		case 0x26: return 0x000A; // 9 (
		case 0x27: return 0x000B; // 0 )
		case 0x28: return 0x001C; // Return
		case 0x29: return 0x0001; // Escape
		case 0x2A: return 0x000E; // Backspace
		case 0x2B: return 0x000F; // Tab
		case 0x2C: return 0x0039; // Space
		case 0x2D: return 0x000C; // - _
		case 0x2E: return 0x000D; // = +
		case 0x2F: return 0x001A; // [ case 
		case 0x30: return 0x001B; // ] }
		case 0x31: return 0x002B; // \ |
		case 0x32: return 0x002B; // Europe 1 (Note 2)
		case 0x33: return 0x0027; // ; :
		case 0x34: return 0x0028; // ' "
		case 0x35: return 0x0029; // ` ~
		case 0x36: return 0x0033; // , <
		case 0x37: return 0x0034; // . >
		case 0x38: return 0x0035; // / ?
		case 0x39: return 0x003A; // Caps Lock
		case 0x3A: return 0x003B; // F1
		case 0x3B: return 0x003C; // F2
		case 0x3C: return 0x003D; // F3
		case 0x3D: return 0x003E; // F4
		case 0x3E: return 0x003F; // F5
		case 0x3F: return 0x0040; // F6
		case 0x40: return 0x0041; // F7
		case 0x41: return 0x0042; // F8
		case 0x42: return 0x0043; // F9
		case 0x43: return 0x0044; // F10
		case 0x44: return 0x0057; // F11
		case 0x45: return 0x0058; // F12
		case 0x46: return 0xE037; // Print Screen (Note 1)
		case 0x47: return 0x0046; // Scroll Lock
		case 0x49: return 0xE052; // Insert (Note 1)
		case 0x4A: return 0xE047; // Home (Note 1)
		case 0x4B: return 0xE049; // Page Up (Note 1)
		case 0x4C: return 0xE053; // Delete (Note 1)
		case 0x4D: return 0xE04F; // End (Note 1)
		case 0x4E: return 0xE051; // Page Down (Note 1)
		case 0x4F: return 0xE04D; // Right Arrow (Note 1)
		case 0x50: return 0xE04B; // Left Arrow (Note 1)
		case 0x51: return 0xE050; // Down Arrow (Note 1)
		case 0x52: return 0xE048; // Up Arrow (Note 1)
		case 0x53: return 0x0045; // Num Lock
		case 0x54: return 0xE035; // Keypad / (Note 1)
		case 0x55: return 0x0037; // Keypad *
		case 0x56: return 0x004A; // Keypad -
		case 0x57: return 0x004E; // Keypad +
		case 0x58: return 0xE01C; // Keypad Enter
		case 0x59: return 0x004F; // Keypad 1 End
		case 0x5A: return 0x0050; // Keypad 2 Down
		case 0x5B: return 0x0051; // Keypad 3 PageDn
		case 0x5C: return 0x004B; // Keypad 4 Left
		case 0x5D: return 0x004C; // Keypad 5
		case 0x5E: return 0x004D; // Keypad 6 Right
		case 0x5F: return 0x0047; // Keypad 7 Home
		case 0x60: return 0x0048; // Keypad 8 Up
		case 0x61: return 0x0049; // Keypad 9 PageUp
		case 0x62: return 0x0052; // Keypad 0 Insert
		case 0x63: return 0x0053; // Keypad . Delete
		case 0x64: return 0x0056; // Europe 2 (Note 2)
		case 0x65: return 0xE05D; // App
		case 0x67: return 0x0059; // Keypad =
		case 0x68: return 0x005D; // F13
		case 0x69: return 0x005E; // F14
		case 0x6A: return 0x005F; // F15
		case 0x85: return 0x007E; // Keypad , (Brazilian Keypad .)
		case 0x87: return 0x0073; // Keyboard Int'l 1 ろ (Ro)
		case 0x88: return 0x0070; // Keyboard Int'l 2 かたかな ひらがな ローマ字 (Katakana/Hiragana)
		case 0x89: return 0x007D; // Keyboard Int'l 3 ￥ (Yen)
		case 0x8A: return 0x0079; // Keyboard Int'l 4 前候補 変換 (次候補) 全候補 (Henkan)
		case 0x8B: return 0x007B; // Keyboard Int'l 5 無変換 (Muhenkan)
		case 0x8C: return 0x005C; // Keyboard Int'l 6 (PC9800 Keypad , )
		case 0x90: return 0x00F2; // Keyboard Lang 1 한/영 (Hanguel/English)
		case 0x91: return 0x00F1; // Keyboard Lang 2 한자 (Hanja)
		case 0x92: return 0x0078; // Keyboard Lang 3 かたかな (Katakana)
		case 0x93: return 0x0077; // Keyboard Lang 4 ひらがな (Hiragana)
		case 0x94: return 0x0076; // Keyboard Lang 5 半角/全角 (Zenkaku/Hankaku)
		case 0xE0: return 0x001D; // Left Control
		case 0xE1: return 0x002A; // Left Shift
		case 0xE2: return 0x0038; // Left Alt
		case 0xE3: return 0xE05B; // Left GUI
		case 0xE4: return 0xE01D; // Right Control
		case 0xE5: return 0x0036; // Right Shift
		case 0xE6: return 0xE038; // Right Alt
		case 0xE7: return 0xE05C; // Right GUI
		}
		return 0;
	}

	[[nodiscard]] constexpr uint16_t usb_hid_scancode_to_ps2_scancode(UsbHidScancode scancode) noexcept
	{
		return usb_hid_scancode_to_ps2_scancode(static_cast<uint8_t>(scancode));
	}
}
