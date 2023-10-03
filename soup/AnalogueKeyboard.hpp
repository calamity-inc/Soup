#pragma once

#include "base.hpp"
#if SOUP_WINDOWS

#include <vector>

#include "scancodes.hpp"
#include "UsbHid.hpp"

namespace soup
{
	struct AnalogueKeyboard
	{
		const char* name;
		UsbHid hid;
		bool disconnected = false;

		[[nodiscard]] static std::vector<AnalogueKeyboard> getAll();

		class ActiveKey
		{
		public:
			uint16_t scancode;
			uint8_t value;

			[[nodiscard]] uint8_t getSoupKey() const noexcept
			{
				if (hasUsbHidScancode())
				{
					return usb_hid_scancode_to_soup_key(getUsbHidScancode());
				}
				else
				{
					// Correct for Wooting Two & Wooting Two HE
					if (scancode == 0x409)
					{
						return KEY_FN;
					}
				}
				return KEY_NONE;
			}

			[[nodiscard]] bool hasUsbHidScancode() const noexcept
			{
				return scancode <= 0xFF;
			}
			
			[[nodiscard]] UsbHidScancode getUsbHidScancode() const noexcept
			{
				return (UsbHidScancode)scancode;
			}

			[[nodiscard]] uint16_t getPs2Scancode() const noexcept
			{
				return hasUsbHidScancode() ? usb_hid_scancode_to_ps2_scancode(scancode) : 0;
			}

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

			[[nodiscard]] float getFValue() const noexcept
			{
				return static_cast<float>(value) / 255.0f;
			}

		private:
			[[nodiscard]] int getVkPrecheck() const noexcept
			{
				// MapVirtualKeyA would return VK_INSERT instead of VK_NUMPAD0 etc.
				switch (scancode)
				{
				case HID_NUMPAD0: return VK_NUMPAD0;
				case HID_NUMPAD1: return VK_NUMPAD1;
				case HID_NUMPAD2: return VK_NUMPAD2;
				case HID_NUMPAD3: return VK_NUMPAD3;
				case HID_NUMPAD4: return VK_NUMPAD4;
				case HID_NUMPAD5: return VK_NUMPAD5;
				case HID_NUMPAD6: return VK_NUMPAD6;
				case HID_NUMPAD7: return VK_NUMPAD7;
				case HID_NUMPAD8: return VK_NUMPAD8;
				case HID_NUMPAD9: return VK_NUMPAD9;
				case HID_NUMPAD_DECIMAL: return VK_DECIMAL;
				default:;
				}
				return 0;
			}
		};

		// Polls the latest report from the device and parses it. (Blocking)
		[[nodiscard]] std::vector<ActiveKey> getActiveKeys();
	};
}

#endif
