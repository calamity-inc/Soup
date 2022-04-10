#pragma once

#include <string>
#include <vector>

#include "base.hpp"
#include "rgb.hpp"

namespace soup
{
	class canvas
	{
	public:
		int width;
		int height;
		std::vector<rgb> pixels{};

		canvas() noexcept = default;

		canvas(int width, int height)
			: width(width), height(height)
		{
			resize(width, height);
		}

		void resize(int width, int height);

		void fill(const rgb colour);
		void set(int x, int y, rgb colour);
		[[nodiscard]] rgb get(int x, int y) const;

		[[nodiscard]] std::string toStringx1() const;
		[[nodiscard]] std::u16string toStringx2();
	private:
		[[nodiscard]] std::u16string toStringx2_impl() const;
		[[nodiscard]] static char16_t x2chunkToChar(uint8_t chunkset) noexcept;

		void ensureWidthAndHeightAreEven();
		void resizeWidth(int new_width);

	public:
		[[nodiscard]] std::string toPPM() const; // Bit of a niche format, but dead simple to write. You can load images of this type with GIMP.
	};
}
