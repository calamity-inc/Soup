#pragma once

#include <string>
#include <vector>

#include "Rgba.hpp"

NAMESPACE_SOUP
{
	// Canvas with an alpha channel.
	struct ACanvas
	{
		unsigned int width = 0;
		unsigned int height = 0;
		std::vector<Rgba> pixels{};

		ACanvas() noexcept = default;

		ACanvas(unsigned int size)
			: ACanvas(size, size)
		{
		}

		ACanvas(unsigned int width, unsigned int height)
			: width(width), height(height)
		{
			pixels.resize(width * height);
		}

		void set(unsigned int x, unsigned int y, Rgba colour) noexcept
		{
			SOUP_IF_LIKELY(x < width && y < height)
			{
				pixels.at(x + (y * width)) = colour;
			}
		}

		[[nodiscard]] Rgba get(unsigned int x, unsigned int y) const
		{
			return pixels.at(x + (y * width));
		}

		void addRect(unsigned int x_offset, unsigned int y_offset, unsigned int width, unsigned int height, Rgba colour)
		{
			for (unsigned int y = 0; y != height; ++y)
			{
				for (unsigned int x = 0; x != width; ++x)
				{
					set(x + x_offset, y + y_offset, colour);
				}
			}
		}

		[[nodiscard]] std::string toPam() const; // Bit of a niche format, but dead simple to write. You can convert it with ffmpeg.
	};
}
