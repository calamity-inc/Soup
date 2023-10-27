#pragma once

#include "kbRgb.hpp"

#include "hwHid.hpp"

namespace soup
{
	class kbRgbWooting final : public kbRgb
	{
	public:
		bool has_numpad = false;
		hwHid hid;
	protected:
		bool inited = false;

	public:
		kbRgbWooting(const char* name, bool has_numpad, hwHid&& hid)
			: kbRgb(name), has_numpad(has_numpad), hid(std::move(hid))
		{
		}

		~kbRgbWooting() final;

		[[nodiscard]] bool controlsDevice(const hwHid& hid) const noexcept final;

		void deinit() final;

		void setKey(Key key, Rgb colour) final;
		void setKeys(const Rgb(&colours)[NUM_KEYS]) final;
		void setAllKeys(Rgb colour) final;

		[[nodiscard]] uint8_t getNumColumns() const noexcept final;
		[[nodiscard]] Key mapPosToKey(uint8_t row, uint8_t column) const noexcept final;

		[[nodiscard]] float getBrightness();

		static void mapCanvasToKeys(Rgb(&keys)[NUM_KEYS], const Canvas& c);
	};
}
