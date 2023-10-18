#pragma once

#include <vector>

#include "Key.hpp"
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

		[[nodiscard]] uint8_t getNumRows() const noexcept { return 6; }
		[[nodiscard]] virtual uint8_t getNumColumns() const noexcept = 0;
		[[nodiscard]] virtual Key getKeyForPos(uint8_t row, uint8_t column) const noexcept = 0;
		void mapPosToKeys(Rgb(&keys)[NUM_KEYS], Rgb* data, uint8_t rows, uint8_t columns);
	};
}
