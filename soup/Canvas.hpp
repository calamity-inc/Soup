#pragma once

#include <string>
#include <vector>

#include "base.hpp"
#include "fwd.hpp"

#include "Rgb.hpp"

namespace soup
{
	class Canvas
	{
	public:
		size_t width;
		size_t height;
		std::vector<Rgb> pixels{};

		Canvas() noexcept = default;

		Canvas(size_t size)
			: Canvas(size, size)
		{
		}

		Canvas(size_t width, size_t height)
			: width(width), height(height)
		{
			pixels.resize(width * height);
		}

		Canvas(size_t width, size_t height, const std::vector<bool>& black_white_data)
			: Canvas(width, height)
		{
			loadBlackWhiteData(black_white_data);
		}

		void fill(const Rgb colour);
		void loadBlackWhiteData(const std::vector<bool>& black_white_data);
		void set(size_t x, size_t y, Rgb colour);
		[[nodiscard]] Rgb get(size_t x, size_t y) const;
		[[nodiscard]] const Rgb& ref(size_t x, size_t y) const;

		void addText(size_t x, size_t y, const std::string& text, const RasterFont& font);
		void addText(size_t x, size_t y, const std::u32string& text, const RasterFont& font);
		void addCanvas(size_t x_offset, size_t y_offset, const Canvas& b);
		void addRect(size_t x_offset, size_t y_offset, size_t width, size_t height, Rgb colour);

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
