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

		canvas(int size)
			: canvas(size, size)
		{
		}

		canvas(int width, int height)
			: width(width), height(height)
		{
			resize(width, height);
		}

		void resize(int width, int height);

		void fill(const rgb colour);
		void set(int x, int y, rgb colour);
		[[nodiscard]] rgb get(int x, int y) const;
		[[nodiscard]] const rgb& ref(size_t x, size_t y) const;

		[[nodiscard]] std::string toString(bool explicit_nl = false) const;
		[[nodiscard]] std::string toStringDoublewidth(bool explicit_nl = false) const;
		[[nodiscard]] std::u16string toStringDownsampled(bool explicit_nl = false);
		[[nodiscard]] std::u16string toStringDownsampledDoublewidth(bool explicit_nl = false);
	private:
		[[nodiscard]] static char16_t downsampleChunkToChar(uint8_t chunkset) noexcept;

		void ensureWidthAndHeightAreEven();
		void ensureHeightIsEven();
		void resizeWidth(int new_width);

	public:
		[[nodiscard]] std::string toPPM() const; // Bit of a niche format, but dead simple to write. You can load images of this type with GIMP.
	};
}
