#include "kbRgbWooting.hpp"

namespace soup
{
	enum class WootingCommand : uint8_t
	{
		Ping = 0,
		GetVersion = 1,
		ResetToBootloader = 2,
		GetSerial = 3,
		GetRgbProfileCount = 4,
		REMOVED_GetCurrentRgbProfileIndex = 5,
		REMOVED_GetRgbMainProfile = 6,
		ReloadProfile0 = 7,
		SaveRgbProfile = 8,
		GetDigitalProfilesCount = 9,
		GetAnalogProfilesCount = 10,
		GetCurrentKeyboardProfileIndex = 11,
		GetDigitalProfile = 12,
		GetAnalogProfileMainPart = 13,
		GetAnalogProfileCurveChangeMapPart1 = 14,
		GetAnalogProfileCurveChangeMapPart2 = 15,
		GetNumberOfKeys = 16,
		GetMainMappingProfile = 17,
		GetFunctionMappingProfile = 18,
		GetDeviceConfig = 19,
		GetAnalogValues = 20,
		KeysOff = 21,
		KeysOn = 22,
		ActivateProfile = 23,
		getDKSProfile = 24,
		doSoftReset = 25,
		REMOVED_GetRgbColorsPart1 = 26,
		REMOVED_GetRgbColorsPart2 = 27,
		REMOVED_GetRgbEffects = 28,
		RefreshRgbColors = 29,
		WootDevSingleColor = 30,
		WootDevResetColor = 31,
		WootDevResetAll = 32,
		WootDevInit = 33,
		REMOVED_GetRgbProfileBase = 34,
		GetRgbProfileColorsPart1 = 35,
		GetRgbProfileColorsPart2 = 36,
		REMOVED_GetRgbProfileEffect = 37,
		ReloadProfile = 38,
		GetKeyboardProfile = 39,
		GetGamepadMapping = 40,
		GetGamepadProfile = 41,
		SaveKeyboardProfile = 42,
		ResetSettings = 43,
		SetRawScanning = 44,
		StartXinputDetection = 45,
		StopXinputDetection = 46,
		SaveDKSProfile = 47,
		GetMappingProfile = 48,
		GetActuationProfile = 49,
		GetRgbProfileCore = 50,
		GetGlobalSettings = 51,
		GetAKCProfile = 52,
		SaveAKCProfile = 53,
		GetRapidTriggerProfile = 54,
		GetProfileMetadata = 55,
		IsFLashChipConnected = 56,
		GetRgbLayer = 57,
	};

	enum class WootingReport : uint8_t
	{
		REMOVED_RgbMainPart = 0,
		REMOVED_DigitalProfileMainPart = 1,
		REMOVED_AnalogProfileMainPart = 2,
		REMOVED_AnalogProfileCurveChangeMapPart1 = 3,
		REMOVED_AnalogProfileCurveChangeMapPart2 = 4,
		REMOVED_MainMappingProfile = 5,
		REMOVED_FunctionMappingProfile = 6,
		DeviceConfig = 7,
		SetDKSProfile = 8,
		RgbColorsPart = 9,
		REMOVED_RgbEffects = 10,
		WootDevRawReport = 11,
		SerialNumber = 12,
		REMOVED_RgbProfileBase = 13,
		RgbProfileColorsPart1 = 14,
		RgbProfileColorsPart2 = 15,
		REMOVED_RgbProfileEffect = 16,
		KeyboardProfile = 17,
		GamepadMapping = 18,
		GamepadProfile = 19,
		MappingProfile = 20,
		ActuationProfile = 21,
		RgbProfileCore = 22,
		GlobalSettings = 23,
		AKCProfile = 24,
		RapidTriggerProfile = 25,
		ProfileMetadata = 26,
		RgbLayer = 27,
	};

	[[nodiscard]] static uint16_t encodeColor(uint8_t red, uint8_t green, uint8_t blue)
	{
		uint16_t encoded = 0;

		encoded |= (red & 0xf8) << 8;
		encoded |= (green & 0xfc) << 3;
		encoded |= (blue & 0xf8) >> 3;

		return encoded;
	}

	// https://github.com/WootingKb/wooting-rgb-sdk/blob/master/resources/keyboard-matrix-rows-columns.png
	[[nodiscard]] static uint8_t getKeyForPos(uint8_t row, uint8_t column)
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
			case 13: return KEY_BACKSLASH;
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
			case 12: return KEY_BACKSLASH;
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
			case 1: return KEY_INTL_BACKSLASH;
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

	[[nodiscard]] static const Rgb& getColourForPos(const Rgb(&colours)[NUM_KEYS], uint8_t row, uint8_t column)
	{
		if (auto sk = getKeyForPos(row, column))
		{
			return colours[sk];
		}
		return Rgb::BLACK;
	}

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
		SOUP_UNUSED(hid.pollReport());
	}

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
		SOUP_UNUSED(hid.pollReport());
	}

	void kbRgbWooting::setKeys(const Rgb(&colours)[NUM_KEYS])
	{
		Buffer buf(256 + 1);
		buf.push_back(/* 0 */ 0); // HID report index
		buf.push_back(/* 1 */ 0xD0); // Magic word
		buf.push_back(/* 2 */ 0xDA); // Magic word
		buf.push_back(/* 3 */ (uint8_t)WootingReport::WootDevRawReport);
		for (uint8_t row = 0; row != 6; ++row)
		{
			for (uint8_t column = 0; column != 21; ++column)
			{
				const Rgb& colour = getColourForPos(colours, row, column);
				auto encoded = encodeColor(colour.r, colour.g, colour.b);
				buf.push_back(encoded & 0xff);
				buf.push_back(encoded >> 8);
			}
		}
		hid.sendReport(std::move(buf));
	}

	void kbRgbWooting::setAllKeys(Rgb colour)
	{
		Buffer buf(256 + 1);
		buf.push_back(/* 0 */ 0); // HID report index
		buf.push_back(/* 1 */ 0xD0); // Magic word
		buf.push_back(/* 2 */ 0xDA); // Magic word
		buf.push_back(/* 3 */ (uint8_t)WootingReport::WootDevRawReport);
		for (uint8_t row = 0; row != 6; ++row)
		{
			for (uint8_t column = 0; column != 21; ++column)
			{
				auto encoded = encodeColor(colour.r, colour.g, colour.b);
				buf.push_back(encoded & 0xff);
				buf.push_back(encoded >> 8);
			}
		}
		hid.sendReport(std::move(buf));
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
		auto buf = hid.pollReport();
		return ((float)(uint8_t)buf.at(8) / 255.0f * 100.0f);
	}
}
