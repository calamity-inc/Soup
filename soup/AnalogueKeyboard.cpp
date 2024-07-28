#include "AnalogueKeyboard.hpp"

#include "BufferRefReader.hpp"
#include "DigitalKeyboard.hpp"
#include "HidScancode.hpp"
#include "macros.hpp" // COUNT
#include "NamedMutex.hpp"

NAMESPACE_SOUP
{
	[[nodiscard]] static std::string checkDeviceName(const hwHid& hid)
	{
		// Wooting, https://github.com/WootingKb/wooting-analog-sdk/blob/develop/wooting-analog-plugin/src/lib.rs
		if (hid.vendor_id == 0x31E3)
		{
			if (hid.usage_page == 0xFF54)
			{
				return hid.getProductName();
			}
		}
		else if (hid.vendor_id == 0x03EB)
		{
			if (hid.usage_page == 0xFF54)
			{
				if (hid.product_id == 0xFF01)
				{
					return "Wooting One (Old Firmware)";
				}
				else if (hid.product_id == 0xFF02)
				{
					return "Wooting Two (Old Firmware)";
				}
			}
		}
		// Razer
		else if (hid.vendor_id == 0x1532)
		{
			if (hid.product_id == 0x0266)
			{
				if (hid.hasReportId(7))
				{
					return "Razer Huntsman V2 Analog";
				}
			}
			else if (hid.product_id == 0x0282)
			{
				if (hid.hasReportId(7))
				{
					return "Razer Huntsman Mini Analog"; // Untested
				}
			}
			else if (hid.product_id == 0x02a6)
			{
				if (hid.hasReportId(11))
				{
					return "Razer Huntsman V3 Pro";
				}
			}
			else if (hid.product_id == 0x02a7)
			{
				if (hid.hasReportId(11))
				{
					return "Razer Huntsman V3 Pro Tenkeyless";
				}
			}
			else if (hid.product_id == 0x02b0)
			{
				if (hid.hasReportId(11))
				{
					return "Razer Huntsman V3 Pro Mini";
				}
			}
		}
		// Keychron
		else if (hid.vendor_id == 0x3434)
		{
			if (hid.usage == 0x61 && hid.usage_page == 0xFF60)
			{
				if (hid.product_id == 0x0b10)
				{
					return "Keychron Q1 HE";
				}
			}
		}

		return {};
	}

	std::vector<AnalogueKeyboard> AnalogueKeyboard::getAll(bool include_no_permission)
	{
		auto devices = hwHid::getAll();

		// For Razer keyboards, Synapse needs to be active for the keyboard to send analogue reports.
		// However, for the Huntsman V2 Analog, we can enable analogue reports ourselves by setting the device mode to 3.
		// This does work for V3 Pro (+ TKL + Mini) as well, but causes them to stop sending digital reports without Synapse. More research needed.
		for (auto& hid : devices)
		{
			if (hid.vendor_id == 0x1532
				&& hid.product_id == 0x0266
#if SOUP_WINDOWS
				&& hid.feature_report_byte_length == 91
#endif
				&& hid.havePermission()
				)
			{
				Buffer buf;
				buf.append(
					"\x00" // HID report index
					"\x00\x1f\x00\x00\x00\x02\x00\x04\x03\x00\x00\x00\x00\x00\x00\x00" \
					"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
					"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
					"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
					"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
					"\x00\x00\x00\x00\x00\x00\x00\x00\x05\x00"
					, 91);
				hid.sendFeatureReport(std::move(buf));
				//hid.receiveFeatureReport(buf);
			}
		}

		std::vector<AnalogueKeyboard> res{};
		for (auto& hid : devices)
		{
			if (include_no_permission || hid.havePermission())
			{
				// Check if this is a supported device and the right interface for it
				if (auto name = checkDeviceName(hid); !name.empty())
				{
					res.emplace_back(AnalogueKeyboard{
						std::move(name),
						std::move(hid),
						hid.vendor_id == 0x1532 // Has context key? Only true for Razer.
					});
				}
			}
		}
		return res;
	}

	[[nodiscard]] static Key razer_scancode_to_soup_key(uint8_t scancode) noexcept
	{
		switch (scancode)
		{
		case 0x6E: return KEY_ESCAPE;
		case 0x70: return KEY_F1;
		case 0x71: return KEY_F2;
		case 0x72: return KEY_F3;
		case 0x73: return KEY_F4;
		case 0x74: return KEY_F5;
		case 0x75: return KEY_F6;
		case 0x76: return KEY_F7;
		case 0x77: return KEY_F8;
		case 0x78: return KEY_F9;
		case 0x79: return KEY_F10;
		case 0x7A: return KEY_F11;
		case 0x7B: return KEY_F12;

		case 0x01: return KEY_BACKQUOTE;
		case 0x02: return KEY_1;
		case 0x03: return KEY_2;
		case 0x04: return KEY_3;
		case 0x05: return KEY_4;
		case 0x06: return KEY_5;
		case 0x07: return KEY_6;
		case 0x08: return KEY_7;
		case 0x09: return KEY_8;
		case 0x0A: return KEY_9;
		case 0x0B: return KEY_0;
		case 0x0C: return KEY_MINUS;
		case 0x0D: return KEY_EQUALS;
		case 0x0F: return KEY_BACKSPACE;
		case 0x10: return KEY_TAB;
		case 0x11: return KEY_Q;
		case 0x12: return KEY_W;
		case 0x13: return KEY_E;
		case 0x14: return KEY_R;
		case 0x15: return KEY_T;
		case 0x16: return KEY_Y;
		case 0x17: return KEY_U;
		case 0x18: return KEY_I;
		case 0x19: return KEY_O;
		case 0x1A: return KEY_P;
		case 0x1B: return KEY_BRACKET_LEFT;
		case 0x1C: return KEY_BRACKET_RIGHT;
		case 0x2B: return KEY_ENTER;
		case 0x1E: return KEY_CAPS_LOCK;
		case 0x1F: return KEY_A;
		case 0x20: return KEY_S;
		case 0x21: return KEY_D;
		case 0x22: return KEY_F;
		case 0x23: return KEY_G;
		case 0x24: return KEY_H;
		case 0x25: return KEY_J;
		case 0x26: return KEY_K;
		case 0x27: return KEY_L;
		case 0x28: return KEY_SEMICOLON;
		case 0x29: return KEY_QUOTE;
		case 0x2A: return KEY_BACKSLASH;
		case 0x2C: return KEY_LSHIFT;
		case 0x2D: return KEY_INTL_BACKSLASH;
		case 0x2E: return KEY_Z;
		case 0x2F: return KEY_X;
		case 0x30: return KEY_C;
		case 0x31: return KEY_V;
		case 0x32: return KEY_B;
		case 0x33: return KEY_N;
		case 0x34: return KEY_M;
		case 0x35: return KEY_COMMA;
		case 0x36: return KEY_PERIOD;
		case 0x37: return KEY_SLASH;
		case 0x39: return KEY_RSHIFT;
		case 0x3A: return KEY_LCTRL;
		case 0x7F: return KEY_LMETA;
		case 0x3C: return KEY_LALT;
		case 0x3D: return KEY_SPACE;
		case 0x3E: return KEY_RALT;
		case 0x3B: return KEY_FN;
		case 0x81: return KEY_CTX;
		case 0x40: return KEY_RCTRL;

		case 0x7C: return KEY_PRINT_SCREEN;
		case 0x7D: return KEY_PAUSE;
		case 0x7E: return KEY_SCROLL_LOCK;

		case 0x4B: return KEY_INSERT;
		case 0x50: return KEY_HOME;
		case 0x55: return KEY_PAGE_UP;
		case 0x4C: return KEY_DEL;
		case 0x51: return KEY_END;
		case 0x56: return KEY_PAGE_DOWN;

		case 0x53: return KEY_ARROW_UP;
		case 0x4F: return KEY_ARROW_LEFT;
		case 0x54: return KEY_ARROW_DOWN;
		case 0x59: return KEY_ARROW_RIGHT;

		case 0x5A: return KEY_NUM_LOCK;
		case 0x5F: return KEY_NUMPAD_DIVIDE;
		case 0x64: return KEY_NUMPAD_MULTIPLY;
		case 0x69: return KEY_NUMPAD_SUBTRACT;
		case 0x5B: return KEY_NUMPAD7;
		case 0x60: return KEY_NUMPAD8;
		case 0x65: return KEY_NUMPAD9;
		case 0x6A: return KEY_NUMPAD_ADD;
		case 0x5C: return KEY_NUMPAD4;
		case 0x61: return KEY_NUMPAD5;
		case 0x66: return KEY_NUMPAD6;
		case 0x5D: return KEY_NUMPAD1;
		case 0x62: return KEY_NUMPAD2;
		case 0x67: return KEY_NUMPAD3;
		case 0x6C: return KEY_NUMPAD_ENTER;
		case 0x63: return KEY_NUMPAD0;
		case 0x68: return KEY_NUMPAD_DECIMAL;
		}
		return KEY_NONE;
	}

	struct hwKeyPosition
	{
		Key sk;
		uint8_t row;
		uint8_t column;
	};

	struct hwKeyPosition keychron_keys[] = {
		{ KEY_ESCAPE, 0, 0 },
		{ KEY_F1, 0, 1 },
		{ KEY_F2, 0, 2 },
		{ KEY_F3, 0, 3 },
		{ KEY_F4, 0, 4 },
		{ KEY_F5, 0, 5 },
		{ KEY_F6, 0, 6 },
		{ KEY_F7, 0, 7 },
		{ KEY_F8, 0, 8 },
		{ KEY_F9, 0, 9 },
		{ KEY_F10, 0, 10 },
		{ KEY_F11, 0, 11 },
		{ KEY_F12, 0, 12 },
		{ KEY_DEL, 0, 13 },
		// mute key

		{ KEY_BACKQUOTE, 1, 0 },
		{ KEY_1, 1, 1 },
		{ KEY_2, 1, 2 },
		{ KEY_3, 1, 3 },
		{ KEY_4, 1, 4 },
		{ KEY_5, 1, 5 },
		{ KEY_6, 1, 6 },
		{ KEY_7, 1, 7 },
		{ KEY_8, 1, 8 },
		{ KEY_9, 1, 9 },
		{ KEY_0, 1, 10 },
		{ KEY_MINUS, 1, 11 },
		{ KEY_EQUALS, 1, 12 },
		{ KEY_BACKSPACE, 1, 13 },
		{ KEY_PAGE_UP, 1, 14 },

		{ KEY_TAB, 2, 0 },
		{ KEY_Q, 2, 1 },
		{ KEY_W, 2, 2 },
		{ KEY_E, 2, 3 },
		{ KEY_R, 2, 4 },
		{ KEY_T, 2, 5 },
		{ KEY_Y, 2, 6 },
		{ KEY_U, 2, 7 },
		{ KEY_I, 2, 8 },
		{ KEY_O, 2, 9 },
		{ KEY_P, 2, 10 },
		{ KEY_BRACKET_LEFT, 2, 11 },
		{ KEY_BRACKET_RIGHT, 2, 12 },
		{ KEY_BACKSLASH, 2, 13 },

		{ KEY_CAPS_LOCK, 3, 0 },
		{ KEY_A, 3, 1 },
		{ KEY_S, 3, 2 },
		{ KEY_D, 3, 3 },
		{ KEY_F, 3, 4 },
		{ KEY_G, 3, 5 },
		{ KEY_H, 3, 6 },
		{ KEY_J, 3, 7 },
		{ KEY_K, 3, 8 },
		{ KEY_L, 3, 9 },
		{ KEY_SEMICOLON, 3, 10 },
		{ KEY_QUOTE, 3, 11 },
		{ KEY_ENTER, 3, 12 },

		{ KEY_LSHIFT, 4, 0 },
		{ KEY_Z, 4, 2 },
		{ KEY_X, 4, 3 },
		{ KEY_C, 4, 4 },
		{ KEY_V, 4, 5 },
		{ KEY_B, 4, 6 },
		{ KEY_N, 4, 7 },
		{ KEY_M, 4, 8 },
		{ KEY_COMMA, 4, 9 },
		{ KEY_PERIOD, 4, 10 },
		{ KEY_SLASH, 4, 12 },
		{ KEY_RSHIFT, 4, 13 },

		{ KEY_LCTRL, 5, 0 },
		{ KEY_LMETA, 5, 1 },
		{ KEY_LALT, 5, 2 },
		{ KEY_SPACE, 5, 3 },
		{ KEY_RMETA, 5, 3 },
		{ KEY_FN, 5, 3 },
		{ KEY_RCTRL, 5, 3 },
	};
	static_assert(COUNT(keychron_keys) == 75); // has to match AnalogueKeyboard::buffer

	bool AnalogueKeyboard::isPoll() const noexcept
	{
		return hid.usage_page == 0xFF60; // Keychron
	}

	using ActiveKey = AnalogueKeyboard::ActiveKey;

	std::vector<ActiveKey> AnalogueKeyboard::getActiveKeys()
	{
		std::vector<ActiveKey> keys{};

		if (hid.usage_page == 0xFF60) // Keychron
		{
#if SOUP_WINDOWS
			NamedMutex mtx("KeychronMtx");
			mtx.lock();
			static DigitalKeyboard dkbd;
			dkbd.update();
			bool found = false;
			for (auto& hid : hwHid::getAll())
			{
				if (hid.usage_page == 0xFF60)
				{
					found = true;
#endif
					char data[33];
					memset(data, 0, sizeof(data));
					data[1] = 0xa9; // KC_HE
					data[2] = 0x30; // AMC_GET_REALTIME_TRAVEL
					for (uint8_t i = 0; i != COUNT(keychron_keys); ++i)
					{
						const auto& key = keychron_keys[i];
						if (
#if SOUP_WINDOWS
							dkbd.keys[key.sk] ||
#endif
							keychron.buffer[i] || keychron.state == (i >> 2)
							)
						{
							data[3] = key.row;
							data[4] = key.column;
							hid.sendReport(data, sizeof(data));
							const auto& report = hid.receiveReport();
							SOUP_IF_UNLIKELY (report.empty())
							{
								disconnected = true;
								break;
							}
							keychron.buffer[i] = report.at(2);
						}
						if (keychron.buffer[i] != 0)
						{
							keys.emplace_back(ActiveKey{
								key.sk,
								static_cast<float>(keychron.buffer[i]) / 40.0f
							});
						}
					}
					if (keychron.state++ == (COUNT(keychron_keys) >> 2))
					{
						keychron.state = 0;
					}
#if SOUP_WINDOWS
				}
			}
			mtx.unlock();
			if (!found)
			{
				disconnected = true;
			}
#endif
			return keys;
		}

		const Buffer& report = hid.receiveReport();
		SOUP_IF_UNLIKELY (report.empty())
		{
			//std::cout << "empty report" << std::endl;
			if (hid.vendor_id != 0x1532
				|| ++razer.consecutive_empty_reports == 10
				)
			{
				disconnected = true;
			}
		}
		else
		{
			BufferRefReader r(report);
			if (hid.usage_page == 0xFF54) // Wooting, up to 16 keys
			{
				uint16_t scancode;
				uint8_t value;
				while (r.hasMore()
					&& r.u16_be(scancode)
					&& scancode != 0
					&& r.u8(value)
					)
				{
					Key sk;
					SOUP_IF_UNLIKELY ((scancode >> 8) != 0)
					{
						switch (scancode)
						{
						default: sk = KEY_NONE; break;
						case 0x3B5: sk = KEY_NEXT_TRACK; break;
						case 0x3B6: sk = KEY_PREV_TRACK; break;
						case 0x3B7: sk = KEY_STOP_MEDIA; break;
						case 0x3CD: sk = KEY_PLAY_PAUSE; break;
						case 0x403: sk = KEY_OEM_1; break;
						case 0x404: sk = KEY_OEM_2; break;
						case 0x405: sk = KEY_OEM_3; break;
						case 0x408: sk = KEY_OEM_4; break;
						case 0x409: sk = KEY_FN; break;
						}
					}
					else
					{
						sk = hid_scancode_to_soup_key(static_cast<uint8_t>(scancode));
					}
					SOUP_IF_LIKELY (sk != KEY_NONE)
					{
						// some keys seem to be getting reported multiple times on older firmware, so just use last reported value
						for (auto& key : keys)
						{
							if (key.getSoupKey() == sk)
							{
								key.fvalue = static_cast<float>(value) / 255.0f;
								goto _no_emplace;
							}
						}
						keys.emplace_back(ActiveKey{
							sk,
							static_cast<float>(value) / 255.0f
						});
						_no_emplace:;
					}
				}
			}
			else // Razer
			{
				razer.consecutive_empty_reports = 0;

				//std::cout << string::bin2hex(report.toString()) << std::endl;

				r.skip(1); // ignore report id

				if (hid.product_id >= 0x02a6) // Huntsman V3, up to 15 keys
				{
					uint8_t scancode;
					uint8_t value;
					while (r.hasMore()
						&& r.u8(scancode)
						&& scancode != 0
						&& r.u8(value)
						&& r.skip(1) // unclear, might be something like "priority."
						)
					{
						const auto sk = razer_scancode_to_soup_key(scancode);
						SOUP_IF_LIKELY(sk != KEY_NONE)
						{
							keys.emplace_back(ActiveKey{
								sk,
								static_cast<float>(value) / 255.0f
							});
						}
					}
				}
				else // Huntsman V2, up to 11 keys
				{
					uint8_t scancode;
					uint8_t value;
					while (r.hasMore()
						&& r.u8(scancode)
						&& scancode != 0
						&& r.u8(value)
						)
					{
						const auto sk = razer_scancode_to_soup_key(scancode);
						SOUP_IF_LIKELY(sk != KEY_NONE)
						{
							keys.emplace_back(ActiveKey{
								sk,
								static_cast<float>(value) / 255.0f
							});
						}
					}
				}
			}
		}

		return keys;
	}

	uint8_t AnalogueKeyboard::ActiveKey::getHidScancode() const noexcept
	{
		return soup_key_to_hid_scancode(sk);
	}
}
