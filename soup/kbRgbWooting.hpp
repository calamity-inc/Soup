#pragma once

#include "kbRgb.hpp"

#include "UsbHid.hpp"

namespace soup
{
	struct kbRgbWooting : public kbRgb
	{
		UsbHid hid;

		kbRgbWooting(UsbHid&& hid)
			: hid(std::move(hid))
		{
		}

		void init() final;
		void deinit() final;

		void setKey(uint8_t key, Rgb colour) final;
		void setKeys(const Rgb(&colours)[NUM_KEYS]) final;
		void setAllKeys(Rgb colour) final;

		[[nodiscard]] float getBrightness();
	};
}
