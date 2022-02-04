#pragma once

#include <string>
#include <vector>

#include "rgb.hpp"
#include "stdint.hpp"

namespace soup
{
	class canvas
	{
	private:
		size_t width;
		size_t height;
		std::vector<rgb> pixels{};

	public:
		canvas() noexcept = default;

		canvas(size_t width, size_t height)
			: width(width), height(height)
		{
			pixels.resize(width * height);
		}

		void set(size_t x, size_t y, rgb colour);

		[[nodiscard]] std::u16string toString();
	private:
		[[nodiscard]] static char16_t chunkToChar(uint8_t chunkset) noexcept;

		void ensureWidthAndHeightAreEven();
		void resizeWidth(int new_width);
	};
}
