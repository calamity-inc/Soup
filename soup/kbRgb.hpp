#pragma once

#include <vector>

#include "keys.hpp"
#include "Rgb.hpp"
#include "UniquePtr.hpp"
#include "UsbHid.hpp"

namespace soup
{
	struct kbRgb
	{
		const char* name;
		bool has_numpad = false;
		UsbHid hid;

		kbRgb(const char* name, bool has_numpad, UsbHid&& hid)
			: name(name), has_numpad(has_numpad), hid(std::move(hid))
		{
		}

		[[nodiscard]] static std::vector<UniquePtr<kbRgb>> getAll();

		virtual ~kbRgb() = default;

		virtual void init() = 0;
		virtual void deinit() = 0;

		virtual void setKey(uint8_t key, Rgb colour) = 0;
		virtual void setKeys(const Rgb(&colours)[NUM_KEYS]) = 0;
		virtual void setAllKeys(Rgb colour);
	};
}
