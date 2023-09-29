#include "AnalogueKeyboard.hpp"

#include "Packet.hpp"
#include "StringRefReader.hpp"

namespace soup
{
	[[nodiscard]] static const char* checkDeviceName(const UsbHid& hid)
	{
		// Wooting, https://github.com/WootingKb/wooting-analog-sdk/blob/develop/wooting-analog-plugin/src/lib.rs
		if (hid.usage_page == 0xFF54)
		{
			if (hid.vendor_id == 0x03EB)
			{
				if (hid.product_id == 0xFF01)
				{
					return "Wooting One";
				}
				if (hid.product_id == 0xFF02)
				{
					return "Wooting Two";
				}
			}
			else if (hid.vendor_id == 0x31E3)
			{
				if ((hid.product_id & 0xFFF0) == 0x1100)
				{
					return "Wooting One V2";
				}
				if ((hid.product_id & 0xFFF0) == 0x1200)
				{
					return "Wooting Two V2";
				}
				if ((hid.product_id & 0xFFF0) == 0x1210)
				{
					return "Wooting Lekker";
				}
				if ((hid.product_id & 0xFFF0) == 0x1220)
				{
					return "Wooting Two HE";
				}
				if ((hid.product_id & 0xFFF0) == 0x1230)
				{
					return "Wooting Two HE ARM";
				}
				if ((hid.product_id & 0xFFF0) == 0x1310)
				{
					return "Wooting 60 HE ARM";
				}
			}
		}

		return nullptr;
	}

	std::vector<AnalogueKeyboard> AnalogueKeyboard::getAll()
	{
		std::vector<AnalogueKeyboard> res{};

		for (auto& hid : UsbHid::getAll())
		{
			if (auto name = checkDeviceName(hid))
			{
				res.emplace_back(AnalogueKeyboard{ name, std::move(hid) });
			}
		}

		return res;
	}

	using ActiveKey = AnalogueKeyboard::ActiveKey;

	std::vector<ActiveKey> AnalogueKeyboard::getActiveKeys() const
	{
		std::vector<ActiveKey> keys{};

		auto report = device.pollReport();
		StringRefReader sr(report);
		uint8_t scancode, value;
		while (sr.hasMore()
			&& sr.skip(1) // scancode is sent as 16-bit value, but the high 8 bits always seem to be 0
			&& sr.u8(scancode)
			&& scancode != 0 // report is always same size, but the after active keys is just 0 bytes
			&& sr.u8(value)
			)
		{
			// some keys seem to be getting reported multiple times, so just use last reported value
			for (auto& key : keys)
			{
				if (key.scancode == static_cast<UsbHidScancode>(scancode))
				{
					key.value = value;
					goto _no_emplace;
				}
			}
			keys.emplace_back(ActiveKey{
				static_cast<UsbHidScancode>(scancode),
				value
			});
		_no_emplace:;
		}

		return keys;
	}
}
