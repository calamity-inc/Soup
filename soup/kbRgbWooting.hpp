#pragma once

#include "kbRgb.hpp"

namespace soup
{
	struct kbRgbWooting : public kbRgb
	{
		using kbRgb::kbRgb;

		[[nodiscard]] uint8_t getNumColumns() const noexcept;

		void init() final;
		void deinit() final;

		void setKey(uint8_t key, Rgb colour) final;
		void setKeys(const Rgb(&colours)[NUM_KEYS]) final;
		void setAllKeys(Rgb colour) final;

		[[nodiscard]] float getBrightness();
	};
}
