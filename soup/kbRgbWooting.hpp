#pragma once

#include "kbRgb.hpp"

#include "hwHid.hpp"

namespace soup
{
	class kbRgbWooting final : public kbRgb
	{
	public:
		hwHid hid;
		uint8_t columns;
		bool arm_based = false;
	protected:
		bool inited = false;

	public:
		kbRgbWooting(const char* name, uint8_t columns, bool arm_based, hwHid&& hid)
			: kbRgb(name), hid(std::move(hid)), columns(columns), arm_based(arm_based)
		{
		}

		~kbRgbWooting() final;

		[[nodiscard]] bool controlsDevice(const hwHid& hid) const noexcept final;

		void deinit() final;

		void setKey(Key key, Rgb colour) final;
		void setKeys(const Rgb(&colours)[NUM_KEYS]) final;

		[[nodiscard]] uint8_t getNumColumns() const noexcept final;
		[[nodiscard]] Key mapPosToKey(uint8_t row, uint8_t column) const noexcept final;

		[[nodiscard]] float getBrightness();

		static void mapCanvasToKeys(Rgb(&keys)[NUM_KEYS], const Canvas& c);
	};
}
