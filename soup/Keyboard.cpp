#include "Keyboard.hpp"

#include "kbRgbRazerChroma.hpp"

#if !SOUP_WASM

namespace soup
{
	std::vector<Keyboard> Keyboard::getAll()
	{
		std::vector<Keyboard> kbds{};

		const bool razer_chroma_available = kbRgbRazerChroma::isAvailable();

		for (auto& anlg : AnalogueKeyboard::getAll())
		{
			UniquePtr<kbRgb> rgb;
			if (razer_chroma_available
				&& anlg.hid.vendor_id == 0x1532
				)
			{
				rgb = soup::make_unique<kbRgbRazerChroma>();
			}
			kbds.emplace_back(Keyboard(std::move(anlg), std::move(rgb)));
		}

		// Associate non-Razer analogue keyboards with their respective RGB interface.
		auto rgbifs = kbRgb::getAll(false);
		for (auto& kbd : kbds)
		{
			if (kbd.analogue && !kbd.rgb)
			{
				for (auto i = rgbifs.begin(); i != rgbifs.end(); )
				{
					if ((*i)->controlsDevice(kbd.analogue->hid))
					{
						kbd.rgb = std::move(*i);
						i = rgbifs.erase(i);
					}
					else
					{
						++i;
					}
				}
			}
		}

		// Create a Keyboard instance with Razer Chroma if available & a non-analogue Razer keyboard is detected.
		if (razer_chroma_available)
		{
			for (auto& hid : hwHid::getAll())
			{
				if (hid.vendor_id == 0x1532)
				{
					bool is_non_analogue_keyboard = false;
					// https://github.com/openrazer/openrazer/blob/master/driver/razerkbd_driver.h
					switch (hid.product_id)
					{
					case 0x010D: // USB_DEVICE_ID_RAZER_BLACKWIDOW_ULTIMATE_2012
					case 0x010E: // USB_DEVICE_ID_RAZER_BLACKWIDOW_STEALTH_EDITION
					case 0x010F: // USB_DEVICE_ID_RAZER_ANANSI
					case 0x0111: // USB_DEVICE_ID_RAZER_NOSTROMO
					case 0x0113: // USB_DEVICE_ID_RAZER_ORBWEAVER
					case 0x0118: // USB_DEVICE_ID_RAZER_DEATHSTALKER_ESSENTIAL
					case 0x011A: // USB_DEVICE_ID_RAZER_BLACKWIDOW_ULTIMATE_2013
					case 0x011B: // USB_DEVICE_ID_RAZER_BLACKWIDOW_STEALTH
					case 0x011C: // USB_DEVICE_ID_RAZER_BLACKWIDOW_TE_2014
					case 0x0201: // USB_DEVICE_ID_RAZER_TARTARUS
					case 0x0202: // USB_DEVICE_ID_RAZER_DEATHSTALKER_EXPERT
					case 0x0203: // USB_DEVICE_ID_RAZER_BLACKWIDOW_CHROMA
					case 0x0204: // USB_DEVICE_ID_RAZER_DEATHSTALKER_CHROMA
					case 0x0205: // USB_DEVICE_ID_RAZER_BLADE_STEALTH
					case 0x0207: // USB_DEVICE_ID_RAZER_ORBWEAVER_CHROMA
					case 0x0208: // USB_DEVICE_ID_RAZER_TARTARUS_CHROMA
					case 0x0209: // USB_DEVICE_ID_RAZER_BLACKWIDOW_CHROMA_TE
					case 0x020F: // USB_DEVICE_ID_RAZER_BLADE_QHD
					case 0x0210: // USB_DEVICE_ID_RAZER_BLADE_PRO_LATE_2016
					case 0x0211: // USB_DEVICE_ID_RAZER_BLACKWIDOW_OVERWATCH
					case 0x0214: // USB_DEVICE_ID_RAZER_BLACKWIDOW_ULTIMATE_2016
					case 0x0216: // USB_DEVICE_ID_RAZER_BLACKWIDOW_X_CHROMA
					case 0x0217: // USB_DEVICE_ID_RAZER_BLACKWIDOW_X_ULTIMATE
					case 0x021A: // USB_DEVICE_ID_RAZER_BLACKWIDOW_X_CHROMA_TE
					case 0x021E: // USB_DEVICE_ID_RAZER_ORNATA_CHROMA
					case 0x021F: // USB_DEVICE_ID_RAZER_ORNATA
					case 0x0220: // USB_DEVICE_ID_RAZER_BLADE_STEALTH_LATE_2016
					case 0x0221: // USB_DEVICE_ID_RAZER_BLACKWIDOW_CHROMA_V2
					case 0x0224: // USB_DEVICE_ID_RAZER_BLADE_LATE_2016
					case 0x0225: // USB_DEVICE_ID_RAZER_BLADE_PRO_2017
					case 0x0226: // USB_DEVICE_ID_RAZER_HUNTSMAN_ELITE
					case 0x0227: // USB_DEVICE_ID_RAZER_HUNTSMAN
					case 0x0228: // USB_DEVICE_ID_RAZER_BLACKWIDOW_ELITE
					case 0x022A: // USB_DEVICE_ID_RAZER_CYNOSA_CHROMA
					case 0x022B: // USB_DEVICE_ID_RAZER_TARTARUS_V2
					case 0x022C: // USB_DEVICE_ID_RAZER_CYNOSA_CHROMA_PRO
					case 0x022D: // USB_DEVICE_ID_RAZER_BLADE_STEALTH_MID_2017
					case 0x022F: // USB_DEVICE_ID_RAZER_BLADE_PRO_2017_FULLHD
					case 0x0232: // USB_DEVICE_ID_RAZER_BLADE_STEALTH_LATE_2017
					case 0x0233: // USB_DEVICE_ID_RAZER_BLADE_2018
					case 0x0234: // USB_DEVICE_ID_RAZER_BLADE_PRO_2019
					case 0x0235: // USB_DEVICE_ID_RAZER_BLACKWIDOW_LITE
					case 0x0237: // USB_DEVICE_ID_RAZER_BLACKWIDOW_ESSENTIAL
					case 0x0239: // USB_DEVICE_ID_RAZER_BLADE_STEALTH_2019
					case 0x023A: // USB_DEVICE_ID_RAZER_BLADE_2019_ADV
					case 0x023B: // USB_DEVICE_ID_RAZER_BLADE_2018_BASE
					case 0x023F: // USB_DEVICE_ID_RAZER_CYNOSA_LITE
					case 0x0240: // USB_DEVICE_ID_RAZER_BLADE_2018_MERCURY
					case 0x0241: // USB_DEVICE_ID_RAZER_BLACKWIDOW_2019
					case 0x0243: // USB_DEVICE_ID_RAZER_HUNTSMAN_TE
					case 0x0245: // USB_DEVICE_ID_RAZER_BLADE_MID_2019_MERCURY
					case 0x0246: // USB_DEVICE_ID_RAZER_BLADE_2019_BASE
					case 0x024A: // USB_DEVICE_ID_RAZER_BLADE_STEALTH_LATE_2019
					case 0x024C: // USB_DEVICE_ID_RAZER_BLADE_PRO_LATE_2019
					case 0x024D: // USB_DEVICE_ID_RAZER_BLADE_STUDIO_EDITION_2019
					case 0x024E: // USB_DEVICE_ID_RAZER_BLACKWIDOW_V3
					case 0x0252: // USB_DEVICE_ID_RAZER_BLADE_STEALTH_EARLY_2020
					case 0x0253: // USB_DEVICE_ID_RAZER_BLADE_15_ADV_2020
					case 0x0255: // USB_DEVICE_ID_RAZER_BLADE_EARLY_2020_BASE
					case 0x0256: // USB_DEVICE_ID_RAZER_BLADE_PRO_EARLY_2020
					case 0x0257: // USB_DEVICE_ID_RAZER_HUNTSMAN_MINI
					case 0x0258: // USB_DEVICE_ID_RAZER_BLACKWIDOW_V3_MINI
					case 0x0259: // USB_DEVICE_ID_RAZER_BLADE_STEALTH_LATE_2020
					case 0x025A: // USB_DEVICE_ID_RAZER_BLACKWIDOW_V3_PRO_WIRED
					case 0x025D: // USB_DEVICE_ID_RAZER_ORNATA_V2
					case 0x025E: // USB_DEVICE_ID_RAZER_CYNOSA_V2
					case 0x0269: // USB_DEVICE_ID_RAZER_HUNTSMAN_MINI_JP
					case 0x026A: // USB_DEVICE_ID_RAZER_BOOK_2020
					case 0x026B: // USB_DEVICE_ID_RAZER_HUNTSMAN_V2_TENKEYLESS
					case 0x026C: // USB_DEVICE_ID_RAZER_HUNTSMAN_V2
					case 0x026D: // USB_DEVICE_ID_RAZER_BLADE_15_ADV_EARLY_2021
					case 0x026E: // USB_DEVICE_ID_RAZER_BLADE_17_PRO_EARLY_2021
					case 0x026F: // USB_DEVICE_ID_RAZER_BLADE_15_BASE_EARLY_2021
					case 0x0270: // USB_DEVICE_ID_RAZER_BLADE_14_2021
					case 0x0271: // USB_DEVICE_ID_RAZER_BLACKWIDOW_V3_MINI_WIRELESS
					case 0x0276: // USB_DEVICE_ID_RAZER_BLADE_15_ADV_MID_2021
					case 0x0279: // USB_DEVICE_ID_RAZER_BLADE_17_PRO_MID_2021
					case 0x027A: // USB_DEVICE_ID_RAZER_BLADE_15_BASE_2022
					case 0x028A: // USB_DEVICE_ID_RAZER_BLADE_15_ADV_EARLY_2022
					case 0x028B: // USB_DEVICE_ID_RAZER_BLADE_17_2022
					case 0x028C: // USB_DEVICE_ID_RAZER_BLADE_14_2022
					case 0x028D: // USB_DEVICE_ID_RAZER_BLACKWIDOW_V4_PRO
					case 0x0290: // USB_DEVICE_ID_RAZER_DEATHSTALKER_V2_PRO_WIRELESS
					case 0x0292: // USB_DEVICE_ID_RAZER_DEATHSTALKER_V2_PRO_WIRED
					case 0x0294: // USB_DEVICE_ID_RAZER_ORNATA_V3_X
					case 0x0295: // USB_DEVICE_ID_RAZER_DEATHSTALKER_V2
					case 0x0296: // USB_DEVICE_ID_RAZER_DEATHSTALKER_V2_PRO_TKL_WIRELESS
					case 0x0298: // USB_DEVICE_ID_RAZER_DEATHSTALKER_V2_PRO_TKL_WIRED
					case 0x029E: // USB_DEVICE_ID_RAZER_BLADE_15_2023
					case 0x029F: // USB_DEVICE_ID_RAZER_BLADE_16_2023
					case 0x02A0: // USB_DEVICE_ID_RAZER_BLADE_18_2023
					case 0x02A2: // USB_DEVICE_ID_RAZER_ORNATA_V3_X_ALT
					case 0x0A24: // USB_DEVICE_ID_RAZER_BLACKWIDOW_V3_TK
						is_non_analogue_keyboard = true;
						break;
					}
					if (is_non_analogue_keyboard)
					{
						kbds.emplace_back(Keyboard(soup::make_unique<kbRgbRazerChroma>()));
						break;
					}
				}
			}
		}

		return kbds;
	}
}

#endif
