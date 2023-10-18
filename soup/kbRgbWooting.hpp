#pragma once

#include "kbRgb.hpp"

namespace soup
{
	struct kbRgbWooting : public kbRgb
	{
		using kbRgb::kbRgb;

		void init() final;
		void deinit() final;

		void setKey(uint8_t key, Rgb colour) final;
		void setKeys(const Rgb(&colours)[NUM_KEYS]) final;
		void setAllKeys(Rgb colour) final;

		[[nodiscard]] uint8_t getNumColumns() const noexcept final;
		[[nodiscard]] Key getKeyForPos(uint8_t row, uint8_t column) const noexcept final;

		[[nodiscard]] float getBrightness();
	};
}
