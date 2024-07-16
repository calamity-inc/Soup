#include "kbRgbWooting.hpp"

#include "Canvas.hpp"
#include "wooting_enums.hpp"

NAMESPACE_SOUP
{
	[[nodiscard]] static uint16_t wootingEncodeColour(Rgb colour) noexcept
	{
		return (colour.r & 0xf8) << 8 | (colour.g & 0xfc) << 3 | (colour.b & 0xf8) >> 3;
	}

	[[nodiscard]] static uint8_t mapWootingKey(Key sk) noexcept
	{
		switch (sk)
		{
		case KEY_ESCAPE: return 0 + (32 * 0);
		case KEY_F1: return 2 + (32 * 0);
		case KEY_F2: return 3 + (32 * 0);
		case KEY_F3: return 4 + (32 * 0);
		case KEY_F4: return 5 + (32 * 0);
		case KEY_F5: return 6 + (32 * 0);
		case KEY_F6: return 7 + (32 * 0);
		case KEY_F7: return 8 + (32 * 0);
		case KEY_F8: return 9 + (32 * 0);
		case KEY_F9: return 10 + (32 * 0);
		case KEY_F10: return 11 + (32 * 0);
		case KEY_F11: return 12 + (32 * 0);
		case KEY_F12: return 13 + (32 * 0);
		case KEY_PRINT_SCREEN: return 14 + (32 * 0);
		case KEY_PAUSE: return 15 + (32 * 0);
		case KEY_SCROLL_LOCK: return 16 + (32 * 0);
		case KEY_OEM_1: return 17 + (32 * 0);
		case KEY_OEM_2: return 18 + (32 * 0);
		case KEY_OEM_3: return 19 + (32 * 0);
		case KEY_OEM_4: return 20 + (32 * 0);
		case KEY_BACKQUOTE: return 0 + (32 * 1);
		case KEY_1: return 1 + (32 * 1);
		case KEY_2: return 2 + (32 * 1);
		case KEY_3: return 3 + (32 * 1);
		case KEY_4: return 4 + (32 * 1);
		case KEY_5: return 5 + (32 * 1);
		case KEY_6: return 6 + (32 * 1);
		case KEY_7: return 7 + (32 * 1);
		case KEY_8: return 8 + (32 * 1);
		case KEY_9: return 9 + (32 * 1);
		case KEY_0: return 10 + (32 * 1);
		case KEY_MINUS: return 11 + (32 * 1);
		case KEY_EQUALS: return 12 + (32 * 1);
		case KEY_BACKSPACE: return 13 + (32 * 1);
		case KEY_INSERT: return 14 + (32 * 1);
		case KEY_HOME: return 15 + (32 * 1);
		case KEY_PAGE_UP: return 16 + (32 * 1);
		case KEY_NUM_LOCK: return 17 + (32 * 1);
		case KEY_NUMPAD_DIVIDE: return 18 + (32 * 1);
		case KEY_NUMPAD_MULTIPLY: return 19 + (32 * 1);
		case KEY_NUMPAD_SUBTRACT: return 20 + (32 * 1);
		case KEY_TAB: return 0 + (32 * 2);
		case KEY_Q: return 1 + (32 * 2);
		case KEY_W: return 2 + (32 * 2);
		case KEY_E: return 3 + (32 * 2);
		case KEY_R: return 4 + (32 * 2);
		case KEY_T: return 5 + (32 * 2);
		case KEY_Y: return 6 + (32 * 2);
		case KEY_U: return 7 + (32 * 2);
		case KEY_I: return 8 + (32 * 2);
		case KEY_O: return 9 + (32 * 2);
		case KEY_P: return 10 + (32 * 2);
		case KEY_BRACKET_LEFT: return 11 + (32 * 2);
		case KEY_BRACKET_RIGHT: return 12 + (32 * 2);
		//case KEY_BACKSLASH: return 13 + (32 * 2); // ANSI
		case KEY_DEL: return 14 + (32 * 2);
		case KEY_END: return 15 + (32 * 2);
		case KEY_PAGE_DOWN: return 16 + (32 * 2);
		case KEY_NUMPAD7: return 17 + (32 * 2);
		case KEY_NUMPAD8: return 18 + (32 * 2);
		case KEY_NUMPAD9: return 19 + (32 * 2);
		case KEY_NUMPAD_ADD: return 20 + (32 * 2);
		case KEY_CAPS_LOCK: return 0 + (32 * 3);
		case KEY_A: return 1 + (32 * 3);
		case KEY_S: return 2 + (32 * 3);
		case KEY_D: return 3 + (32 * 3);
		case KEY_F: return 4 + (32 * 3);
		case KEY_G: return 5 + (32 * 3);
		case KEY_H: return 6 + (32 * 3);
		case KEY_J: return 7 + (32 * 3);
		case KEY_K: return 8 + (32 * 3);
		case KEY_L: return 9 + (32 * 3);
		case KEY_SEMICOLON: return 10 + (32 * 3);
		case KEY_QUOTE: return 11 + (32 * 3);
		//case KEY_BACKSLASH: return 12 + (32 * 3); // ISO
		case KEY_ENTER: return 13 + (32 * 3);
		case KEY_NUMPAD4: return 17 + (32 * 3);
		case KEY_NUMPAD5: return 18 + (32 * 3);
		case KEY_NUMPAD6: return 19 + (32 * 3);
		case KEY_LSHIFT: return 0 + (32 * 4);
		case KEY_INTL_BACKSLASH: return 1 + (32 * 4); // ISO
		case KEY_Z: return 2 + (32 * 4);
		case KEY_X: return 3 + (32 * 4);
		case KEY_C: return 4 + (32 * 4);
		case KEY_V: return 5 + (32 * 4);
		case KEY_B: return 6 + (32 * 4);
		case KEY_N: return 7 + (32 * 4);
		case KEY_M: return 8 + (32 * 4);
		case KEY_COMMA: return 9 + (32 * 4);
		case KEY_PERIOD: return 10 + (32 * 4);
		case KEY_SLASH: return 11 + (32 * 4);
		case KEY_RSHIFT: return 13 + (32 * 4);
		case KEY_ARROW_UP: return 15 + (32 * 4);
		case KEY_NUMPAD1: return 17 + (32 * 4);
		case KEY_NUMPAD2: return 18 + (32 * 4);
		case KEY_NUMPAD3: return 19 + (32 * 4);
		case KEY_NUMPAD_ENTER: return 20 + (32 * 4);
		case KEY_LCTRL: return 0 + (32 * 5);
		case KEY_LMETA: return 1 + (32 * 5);
		case KEY_LALT: return 2 + (32 * 5);
		case KEY_SPACE: return 6 + (32 * 5);
		case KEY_RALT: return 10 + (32 * 5);
		case KEY_RMETA: return 11 + (32 * 5);
		case KEY_FN: return 12 + (32 * 5);
		case KEY_RCTRL: return 13 + (32 * 5);
		case KEY_ARROW_LEFT: return 14 + (32 * 5);
		case KEY_ARROW_DOWN: return 15 + (32 * 5);
		case KEY_ARROW_RIGHT: return 16 + (32 * 5);
		case KEY_NUMPAD0: return 18 + (32 * 5);
		case KEY_NUMPAD_DECIMAL: return 19 + (32 * 5);
		default:;
		}
		return 255;
	}

	kbRgbWooting::~kbRgbWooting()
	{
		if (inited)
		{
			deinit();
		}
	}

	bool kbRgbWooting::controlsDevice(const hwHid& hid) const noexcept
	{
		return this->hid.isSamePhysicalDeviceAs(hid);
	}

	// Not needed at all, the firmware does this implicitly when a colour is set.
#if false
	void kbRgbWooting::init()
	{
		{
			Buffer buf(8);
			buf.push_back(/* 0 */ 0); // HID report index
			buf.push_back(/* 1 */ 0xD0); // Magic word
			buf.push_back(/* 2 */ 0xDA); // Magic word
			buf.push_back(/* 3 */ (uint8_t)WootingCommand::WootDevInit);
			buf.push_back(/* 4 */ 0);
			buf.push_back(/* 5 */ 0);
			buf.push_back(/* 6 */ 0);
			buf.push_back(/* 7 */ 0);
			hid.sendFeatureReport(std::move(buf));
		}
		SOUP_UNUSED(hid.receiveReport());
		inited = true;
	}
#endif

	void kbRgbWooting::deinit()
	{
		{
			Buffer buf(8);
			buf.push_back(/* 0 */ 0); // HID report index
			buf.push_back(/* 1 */ 0xD0); // Magic word
			buf.push_back(/* 2 */ 0xDA); // Magic word
			buf.push_back(/* 3 */ (uint8_t)WootingCommand::WootDevResetAll);
			buf.push_back(/* 4 */ 0);
			buf.push_back(/* 5 */ 0);
			buf.push_back(/* 6 */ 0);
			buf.push_back(/* 7 */ 0);
			hid.sendFeatureReport(std::move(buf));
		}
		SOUP_UNUSED(hid.receiveReport());
		inited = false;
	}

	void kbRgbWooting::setKey(Key key, Rgb colour)
	{
		if (key == KEY_BACKSLASH)
		{
			// We could check the keyboard layout: https://github.com/WootingKb/wooting-rgb-sdk/blob/6b00b1dea85693a0ed6b2fdcee55fd5377f53949/src/wooting-usb.c#L210
		   // ... or we could just blind-fire it. :)
			setKeyImpl(13 + (32 * 2), colour);
			setKeyImpl(12 + (32 * 3), colour);
		}
		else if (auto wk = mapWootingKey(key); wk != 255)
		{
			setKeyImpl(wk, colour);
		}
	}

	void kbRgbWooting::setKeyImpl(uint8_t wk, Rgb colour)
	{
		{
			Buffer buf(8);
			buf.push_back(/* 0 */ 0); // HID report index
			buf.push_back(/* 1 */ 0xD0); // Magic word
			buf.push_back(/* 2 */ 0xDA); // Magic word
			buf.push_back(/* 3 */ (uint8_t)WootingCommand::WootDevSingleColor);
			buf.push_back(/* 4 */ colour.b);
			buf.push_back(/* 5 */ colour.g);
			buf.push_back(/* 6 */ colour.r);
			buf.push_back(/* 7 */ wk);
			hid.sendFeatureReport(std::move(buf));
		}
		SOUP_UNUSED(hid.receiveReport());
		inited = true;
	}

	void kbRgbWooting::setKeys(const Rgb(&colours)[NUM_KEYS])
	{
		Buffer buf(257);
		buf.push_back(/* 0 */ 0); // HID report index
		buf.push_back(/* 1 */ 0xD0); // Magic word
		buf.push_back(/* 2 */ 0xDA); // Magic word
		buf.push_back(/* 3 */ (uint8_t)WootingReport::WootDevRawReport);
		for (uint8_t row = 0; row != 6; ++row)
		{
			for (uint8_t column = 0; column != 21; ++column)
			{
				uint16_t encoded = 0;
				if (auto sk = mapPosToKey(row, column); sk != KEY_NONE)
				{
					const Rgb& colour = colours[sk];
					encoded = wootingEncodeColour(colour);
				}
				buf.push_back(encoded & 0xff);
				buf.push_back(encoded >> 8);
			}
		}
		if (buf.size() < 257)
		{
			buf.insert_back(257 - buf.size(), '\0');
		}
		if (small_report)
		{
			for (uint8_t i = 0; i != 4; ++i)
			{
				buf[i * 64] = 0;
				hid.sendReport(&buf[i * 64], 65);
			}
		}
		else
		{
			hid.sendReport(std::move(buf));
		}
		inited = true;
	}

	uint8_t kbRgbWooting::getNumColumns() const noexcept
	{
		return columns;
	}

	// https://github.com/WootingKb/wooting-rgb-sdk/blob/master/resources/keyboard-matrix-rows-columns.png
	Key kbRgbWooting::mapPosToKey(uint8_t row, uint8_t column) const noexcept
	{
		if (row == 0)
		{
			switch (column)
			{
			case 0: return KEY_ESCAPE;
			case 2: return KEY_F1;
			case 3: return KEY_F2;
			case 4: return KEY_F3;
			case 5: return KEY_F4;
			case 6: return KEY_F5;
			case 7: return KEY_F6;
			case 8: return KEY_F7;
			case 9: return KEY_F8;
			case 10: return KEY_F9;
			case 11: return KEY_F10;
			case 12: return KEY_F11;
			case 13: return KEY_F12;
			case 14: return KEY_PRINT_SCREEN;
			case 15: return KEY_PAUSE;
			case 16: return KEY_SCROLL_LOCK;
			case 17: return KEY_OEM_1;
			case 18: return KEY_OEM_2;
			case 19: return KEY_OEM_3;
			case 20: return KEY_OEM_4;
			}
		}
		else if (row == 1)
		{
			switch (column)
			{
			case 0: return KEY_BACKQUOTE;
			case 1: return KEY_1;
			case 2: return KEY_2;
			case 3: return KEY_3;
			case 4: return KEY_4;
			case 5: return KEY_5;
			case 6: return KEY_6;
			case 7: return KEY_7;
			case 8: return KEY_8;
			case 9: return KEY_9;
			case 10: return KEY_0;
			case 11: return KEY_MINUS;
			case 12: return KEY_EQUALS;
			case 13: return KEY_BACKSPACE;
			case 14: return KEY_INSERT;
			case 15: return KEY_HOME;
			case 16: return KEY_PAGE_UP;
			case 17: return KEY_NUM_LOCK;
			case 18: return KEY_NUMPAD_DIVIDE;
			case 19: return KEY_NUMPAD_MULTIPLY;
			case 20: return KEY_NUMPAD_SUBTRACT;
			}
		}
		else if (row == 2)
		{
			switch (column)
			{
			case 0: return KEY_TAB;
			case 1: return KEY_Q;
			case 2: return KEY_W;
			case 3: return KEY_E;
			case 4: return KEY_R;
			case 5: return KEY_T;
			case 6: return KEY_Y;
			case 7: return KEY_U;
			case 8: return KEY_I;
			case 9: return KEY_O;
			case 10: return KEY_P;
			case 11: return KEY_BRACKET_LEFT;
			case 12: return KEY_BRACKET_RIGHT;
			case 13: return KEY_BACKSLASH; // ANSI
			case 14: return KEY_DEL;
			case 15: return KEY_END;
			case 16: return KEY_PAGE_DOWN;
			case 17: return KEY_NUMPAD7;
			case 18: return KEY_NUMPAD8;
			case 19: return KEY_NUMPAD9;
			case 20: return KEY_NUMPAD_ADD;
			}
		}
		else if (row == 3)
		{
			switch (column)
			{
			case 0: return KEY_CAPS_LOCK;
			case 1: return KEY_A;
			case 2: return KEY_S;
			case 3: return KEY_D;
			case 4: return KEY_F;
			case 5: return KEY_G;
			case 6: return KEY_H;
			case 7: return KEY_J;
			case 8: return KEY_K;
			case 9: return KEY_L;
			case 10: return KEY_SEMICOLON;
			case 11: return KEY_QUOTE;
			case 12: return KEY_BACKSLASH; // ISO
			case 13: return KEY_ENTER;
			case 17: return KEY_NUMPAD4;
			case 18: return KEY_NUMPAD5;
			case 19: return KEY_NUMPAD6;
			}
		}
		else if (row == 4)
		{
			switch (column)
			{
			case 0: return KEY_LSHIFT;
			case 1: return KEY_INTL_BACKSLASH; // ISO
			case 2: return KEY_Z;
			case 3: return KEY_X;
			case 4: return KEY_C;
			case 5: return KEY_V;
			case 6: return KEY_B;
			case 7: return KEY_N;
			case 8: return KEY_M;
			case 9: return KEY_COMMA;
			case 10: return KEY_PERIOD;
			case 11: return KEY_SLASH;
			case 13: return KEY_RSHIFT;
			case 15: return KEY_ARROW_UP;
			case 17: return KEY_NUMPAD1;
			case 18: return KEY_NUMPAD2;
			case 19: return KEY_NUMPAD3;
			case 20: return KEY_NUMPAD_ENTER;
			}
		}
		else if (row == 5)
		{
			switch (column)
			{
			case 0: return KEY_LCTRL;
			case 1: return KEY_LMETA;
			case 2: return KEY_LALT;
			case 6: return KEY_SPACE;
			case 10: return KEY_RALT;
			case 11: return KEY_RMETA;
			case 12: return KEY_FN;
			case 13: return KEY_RCTRL;
			case 14: return KEY_ARROW_LEFT;
			case 15: return KEY_ARROW_DOWN;
			case 16: return KEY_ARROW_RIGHT;
			case 18: return KEY_NUMPAD0;
			case 19: return KEY_NUMPAD_DECIMAL;
			}
		}
		return KEY_NONE;
	}

	// Works on my Wooting Two HE
	float kbRgbWooting::getBrightness()
	{
		{
			Buffer buf(8);
			buf.push_back(/* 0 */ 0); // HID report index
			buf.push_back(/* 1 */ 0xD0); // Magic word
			buf.push_back(/* 2 */ 0xDA); // Magic word
			buf.push_back(/* 3 */ (uint8_t)WootingCommand::GetRgbProfileCore);
			buf.push_back(/* 4 */ 0);
			buf.push_back(/* 5 */ 0);
			buf.push_back(/* 6 */ 0);
			buf.push_back(/* 7 */ 0);
			hid.sendFeatureReport(std::move(buf));
		}
		const Buffer& buf = hid.receiveReport();
		return ((float)(uint8_t)buf.at(8) / 255.0f * 100.0f);
	}

	// https://github.com/calamity-inc/Soup/blob/senpai/soup/resources/keyboard-map.png
	void kbRgbWooting::mapCanvasToKeys(Rgb(&keys)[NUM_KEYS], const Canvas& c)
	{
		SOUP_ASSERT(c.width >= 229 && c.height >= 65);

		keys[KEY_ESCAPE] = c.getAverageOfArea(0, 0, 10, 10);
		keys[KEY_F1] = c.getAverageOfArea(19, 0, 10, 10);
		keys[KEY_F2] = c.getAverageOfArea(29, 0, 10, 10);
		keys[KEY_F3] = c.getAverageOfArea(39, 0, 10, 10);
		keys[KEY_F4] = c.getAverageOfArea(49, 0, 10, 10);
		keys[KEY_F5] = c.getAverageOfArea(64, 0, 10, 10);
		keys[KEY_F6] = c.getAverageOfArea(74, 0, 10, 10);
		keys[KEY_F7] = c.getAverageOfArea(84, 0, 10, 10);
		keys[KEY_F8] = c.getAverageOfArea(94, 0, 10, 10);
		keys[KEY_F9] = c.getAverageOfArea(109, 0, 10, 10);
		keys[KEY_F10] = c.getAverageOfArea(119, 0, 10, 10);
		keys[KEY_F11] = c.getAverageOfArea(129, 0, 10, 10);
		keys[KEY_F12] = c.getAverageOfArea(139, 0, 10, 10);

		keys[KEY_BACKQUOTE] = c.getAverageOfArea(0, 15, 10, 10);
		keys[KEY_1] = c.getAverageOfArea(10, 15, 10, 10);
		keys[KEY_2] = c.getAverageOfArea(20, 15, 10, 10);
		keys[KEY_3] = c.getAverageOfArea(30, 15, 10, 10);
		keys[KEY_4] = c.getAverageOfArea(40, 15, 10, 10);
		keys[KEY_5] = c.getAverageOfArea(50, 15, 10, 10);
		keys[KEY_6] = c.getAverageOfArea(60, 15, 10, 10);
		keys[KEY_7] = c.getAverageOfArea(70, 15, 10, 10);
		keys[KEY_8] = c.getAverageOfArea(80, 15, 10, 10);
		keys[KEY_9] = c.getAverageOfArea(90, 15, 10, 10);
		keys[KEY_0] = c.getAverageOfArea(100, 15, 10, 10);
		keys[KEY_MINUS] = c.getAverageOfArea(110, 15, 10, 10);
		keys[KEY_EQUALS] = c.getAverageOfArea(120, 15, 10, 10);
		keys[KEY_BACKSPACE] = c.getAverageOfArea(130, 15, 10, 10);

		keys[KEY_TAB] = c.getAverageOfArea(0, 25, 15, 10);
		keys[KEY_Q] = c.getAverageOfArea(15, 25, 10, 10);
		keys[KEY_W] = c.getAverageOfArea(25, 25, 10, 10);
		keys[KEY_E] = c.getAverageOfArea(35, 25, 10, 10);
		keys[KEY_R] = c.getAverageOfArea(45, 25, 10, 10);
		keys[KEY_T] = c.getAverageOfArea(55, 25, 10, 10);
		keys[KEY_Y] = c.getAverageOfArea(65, 25, 10, 10);
		keys[KEY_U] = c.getAverageOfArea(75, 25, 10, 10);
		keys[KEY_I] = c.getAverageOfArea(85, 25, 10, 10);
		keys[KEY_O] = c.getAverageOfArea(95, 25, 10, 10);
		keys[KEY_P] = c.getAverageOfArea(105, 25, 10, 10);
		keys[KEY_BRACKET_LEFT] = c.getAverageOfArea(115, 25, 10, 10);
		keys[KEY_BRACKET_RIGHT] = c.getAverageOfArea(125, 25, 10, 10);

		keys[KEY_ENTER] = c.getAverageOfArea(138, 30, 11, 10);

		keys[KEY_CAPS_LOCK] = c.getAverageOfArea(0, 35, 18, 10);
		keys[KEY_A] = c.getAverageOfArea(18, 35, 10, 10);
		keys[KEY_S] = c.getAverageOfArea(28, 35, 10, 10);
		keys[KEY_D] = c.getAverageOfArea(38, 35, 10, 10);
		keys[KEY_F] = c.getAverageOfArea(48, 35, 10, 10);
		keys[KEY_G] = c.getAverageOfArea(58, 35, 10, 10);
		keys[KEY_H] = c.getAverageOfArea(68, 35, 10, 10);
		keys[KEY_J] = c.getAverageOfArea(78, 35, 10, 10);
		keys[KEY_K] = c.getAverageOfArea(88, 35, 10, 10);
		keys[KEY_L] = c.getAverageOfArea(98, 35, 10, 10);
		keys[KEY_SEMICOLON] = c.getAverageOfArea(108, 35, 10, 10);
		keys[KEY_QUOTE] = c.getAverageOfArea(118, 35, 10, 10);
		keys[KEY_BACKSLASH] = c.getAverageOfArea(128, 35, 10, 10);

		keys[KEY_LSHIFT] = c.getAverageOfArea(0, 45, 12, 10);
		keys[KEY_INTL_BACKSLASH] = c.getAverageOfArea(12, 45, 10, 10);
		keys[KEY_Z] = c.getAverageOfArea(22, 45, 10, 10);
		keys[KEY_X] = c.getAverageOfArea(32, 45, 10, 10);
		keys[KEY_C] = c.getAverageOfArea(42, 45, 10, 10);
		keys[KEY_V] = c.getAverageOfArea(52, 45, 10, 10);
		keys[KEY_B] = c.getAverageOfArea(62, 45, 10, 10);
		keys[KEY_N] = c.getAverageOfArea(72, 45, 10, 10);
		keys[KEY_M] = c.getAverageOfArea(82, 45, 10, 10);
		keys[KEY_COMMA] = c.getAverageOfArea(92, 45, 10, 10);
		keys[KEY_PERIOD] = c.getAverageOfArea(102, 45, 10, 10);
		keys[KEY_SLASH] = c.getAverageOfArea(112, 45, 10, 10);
		keys[KEY_RSHIFT] = c.getAverageOfArea(112, 45, 27, 10);

		keys[KEY_LCTRL] = c.getAverageOfArea(0, 55, 12, 10);
		keys[KEY_LMETA] = c.getAverageOfArea(12, 55, 12, 10);
		keys[KEY_LALT] = c.getAverageOfArea(24, 55, 12, 10);
		keys[KEY_SPACE] = c.getAverageOfArea(58, 55, 11, 10);
		keys[KEY_RALT] = c.getAverageOfArea(101, 55, 12, 10);
		keys[KEY_RMETA] = c.getAverageOfArea(113, 55, 12, 10);
		keys[KEY_FN] = c.getAverageOfArea(125, 55, 12, 10);
		keys[KEY_RCTRL] = c.getAverageOfArea(137, 55, 12, 10);

		keys[KEY_PRINT_SCREEN] = c.getAverageOfArea(154, 0, 10, 10);
		keys[KEY_PAUSE] = c.getAverageOfArea(154 + 10, 0, 10, 10);
		keys[KEY_SCROLL_LOCK] = c.getAverageOfArea(154 + 20, 0, 10, 10);

		keys[KEY_INSERT] = c.getAverageOfArea(154, 15, 10, 10);
		keys[KEY_HOME] = c.getAverageOfArea(154 + 10, 15, 10, 10);
		keys[KEY_PAGE_UP] = c.getAverageOfArea(154 + 20, 15, 10, 10);
		keys[KEY_DEL] = c.getAverageOfArea(154, 25, 10, 10);
		keys[KEY_END] = c.getAverageOfArea(154 + 10, 25, 10, 10);
		keys[KEY_PAGE_DOWN] = c.getAverageOfArea(154 + 20, 25, 10, 10);

		keys[KEY_ARROW_UP] = c.getAverageOfArea(164, 45, 10, 10);
		keys[KEY_ARROW_LEFT] = c.getAverageOfArea(154, 55, 10, 10);
		keys[KEY_ARROW_DOWN] = c.getAverageOfArea(164, 55, 10, 10);
		keys[KEY_ARROW_RIGHT] = c.getAverageOfArea(174, 55, 10, 10);

		keys[KEY_NUM_LOCK] = c.getAverageOfArea(189, 15, 10, 10);
		keys[KEY_NUMPAD_DIVIDE] = c.getAverageOfArea(199, 15, 10, 10);
		keys[KEY_NUMPAD_MULTIPLY] = c.getAverageOfArea(209, 15, 10, 10);
		keys[KEY_NUMPAD_SUBTRACT] = c.getAverageOfArea(219, 15, 10, 10);
		keys[KEY_NUMPAD7] = c.getAverageOfArea(189, 25, 10, 10);
		keys[KEY_NUMPAD8] = c.getAverageOfArea(199, 25, 10, 10);
		keys[KEY_NUMPAD9] = c.getAverageOfArea(209, 25, 10, 10);
		keys[KEY_NUMPAD_ADD] = c.getAverageOfArea(219, 25, 10, 20);
		keys[KEY_NUMPAD4] = c.getAverageOfArea(189, 35, 10, 10);
		keys[KEY_NUMPAD5] = c.getAverageOfArea(199, 35, 10, 10);
		keys[KEY_NUMPAD6] = c.getAverageOfArea(209, 35, 10, 10);
		keys[KEY_NUMPAD1] = c.getAverageOfArea(189, 45, 10, 10);
		keys[KEY_NUMPAD2] = c.getAverageOfArea(199, 45, 10, 10);
		keys[KEY_NUMPAD3] = c.getAverageOfArea(209, 45, 10, 10);
		keys[KEY_NUMPAD_ENTER] = c.getAverageOfArea(219, 45, 10, 20);
		keys[KEY_NUMPAD0] = c.getAverageOfArea(189, 55, 20, 10);
		keys[KEY_NUMPAD_DECIMAL] = c.getAverageOfArea(209, 55, 10, 10);

		keys[KEY_OEM_1] = c.getAverageOfArea(189, 0, 10, 10);
		keys[KEY_OEM_2] = c.getAverageOfArea(199, 0, 10, 10);
		keys[KEY_OEM_3] = c.getAverageOfArea(209, 0, 10, 10);
		keys[KEY_OEM_4] = c.getAverageOfArea(219, 0, 10, 10);
	}

	bool kbRgbWooting::isUwu() const noexcept
	{
		return columns == 7;
	}
}
