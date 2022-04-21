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
		size_t width;
		size_t height;
		std::vector<rgb> pixels{};

		canvas() noexcept = default;

		canvas(size_t size)
			: canvas(size, size)
		{
		}

		canvas(size_t width, size_t height)
			: width(width), height(height)
		{
			resize(width, height);
		}

		void fill(const rgb colour);
		void set(size_t x, size_t y, rgb colour);
		[[nodiscard]] rgb get(size_t x, size_t y) const;
		[[nodiscard]] const rgb& ref(size_t x, size_t y) const;

		void resize(size_t width, size_t height); // Fine for change in height; either trims excess or inserts black pixels below.
		void resizeWidth(size_t new_width); // Resize algorithm for width; either trims excess or inserts black pixels to the right.
		void ensureWidthAndHeightAreEven();
		void ensureHeightIsEven();

		void resizeNearestNeighbour(size_t desired_width, size_t desired_height); // Resizes the canvas and its contents, works for all changes.

		[[nodiscard]] std::string toString(bool explicit_nl = false) const;
		[[nodiscard]] std::string toStringDoublewidth(bool explicit_nl = false) const;
		[[nodiscard]] std::u16string toStringDownsampled(bool explicit_nl = false);
		[[nodiscard]] std::u16string toStringDownsampledDoublewidth(bool explicit_nl = false);
	private:
		[[nodiscard]] static char16_t downsampleChunkToChar(uint8_t chunkset) noexcept;

	public:
		[[nodiscard]] std::string toSvg(size_t scale = 1) const;
		[[nodiscard]] std::string toPpm() const; // Bit of a niche format, but dead simple to write. You can load images of this type with GIMP.
	};
}
