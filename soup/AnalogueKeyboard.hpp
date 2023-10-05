#pragma once

#include "base.hpp"

#include <vector>

#include "hwHid.hpp"
#include "keys.hpp"

namespace soup
{
	struct AnalogueKeyboard
	{
		const char* name;
		hwHid hid;
		bool disconnected = false;

		[[nodiscard]] static std::vector<AnalogueKeyboard> getAll(bool include_no_permission = false);

		class ActiveKey
		{
		public:
			uint8_t sk;
			uint8_t value;

			uint8_t getSoupKey() const noexcept
			{
				return sk;
			}

			[[nodiscard]] uint8_t getHidScancode() const noexcept;

			[[nodiscard]] uint16_t getPs2Scancode() const noexcept
			{
				return soup_key_to_ps2_scancode(sk);
			}

			[[nodiscard]] float getFValue() const noexcept
			{
				return static_cast<float>(value) / 255.0f;
			}

#if SOUP_WINDOWS
			// E.g., if the key to the right of T (KEY_Y) is pressed, will return 'Y'.
			[[nodiscard]] int getVk() const noexcept
			{
				if (int vk = getVkPrecheck())
				{
					return vk;
				}
				return MapVirtualKeyA(getPs2Scancode(), MAPVK_VSC_TO_VK_EX);
			}

			// E.g. if the key to the right of T (KEY_Y) is pressed, may return 'Y' or 'Z' depending on user's layout.
			[[nodiscard]] int getVkTranslated() const noexcept;

		private:
			[[nodiscard]] int getVkPrecheck() const noexcept
			{
				// MapVirtualKeyA would return VK_INSERT instead of VK_NUMPAD0 etc.
				switch (getSoupKey())
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
#endif
		};

		[[nodiscard]] bool havePermission() const noexcept
		{
			return hid.havePermission();
		}

		// Polls the latest report from the device and parses it. (Blocking)
		[[nodiscard]] std::vector<ActiveKey> getActiveKeys();
	};
}
