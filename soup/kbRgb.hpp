#pragma once

#include <vector>

#include "keys.hpp"
#include "Rgb.hpp"
#include "UniquePtr.hpp"
#include "hwHid.hpp"

namespace soup
{
	struct kbRgb
	{
		const char* name;
		bool has_numpad = false;
		hwHid hid;

		kbRgb(const char* name, bool has_numpad, hwHid&& hid)
			: name(name), has_numpad(has_numpad), hid(std::move(hid))
		{
		}

		[[nodiscard]] static std::vector<UniquePtr<kbRgb>> getAll(bool include_no_permission = false);

		[[nodiscard]] bool havePermission() const noexcept
		{
			return hid.havePermission();
		}

		virtual ~kbRgb() = default;

		virtual void init() = 0;
		virtual void deinit() = 0;

		virtual void setKey(uint8_t key, Rgb colour) = 0;
		virtual void setKeys(const Rgb(&colours)[NUM_KEYS]) = 0;
		virtual void setAllKeys(Rgb colour);
	};
}
