#pragma once

#include <cstdint>
#include <optional>

#include "Rgb.hpp"

namespace soup
{
	struct lyoStyle
	{
		bool display_inline = false;

		uint8_t margin_left = 0;
		uint8_t margin_right = 0;
		// margin-top & margin-bottom seemingly ignored with `display: inline`
		uint8_t margin_top = 0;
		uint8_t margin_bottom = 0;

		uint8_t font_size = 0;
		std::optional<Rgb> color = {};
		std::optional<Rgb> background_color = {};

		void setMargin(uint8_t margin) noexcept
		{
			margin_left = margin;
			margin_right = margin;
			margin_top = margin;
			margin_bottom = margin;
		}

		[[nodiscard]] uint8_t getFontScale() const noexcept
		{
			return font_size / 8;
		}

		void overrideWith(const lyoStyle& b) noexcept;

		void reset() noexcept;
	};
}
