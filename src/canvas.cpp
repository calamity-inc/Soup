#include "canvas.hpp"

#include "console.hpp"

namespace soup
{
	void canvas::set(size_t x, size_t y, rgb colour)
	{
		pixels.at(x + (y * width)) = colour;
	}

	std::u16string canvas::toString()
	{
		ensureWidthAndHeightAreEven();
		std::u16string str{};
		str.reserve(width * height);
		for (size_t x = 0; x < width; x += 2)
		{
			for (size_t y = 0; y < height; y += 2)
			{
				uint8_t chunkset = 0;
				rgb bg = pixels.at(x + (y * width));
				rgb fg = bg;
				{
					const rgb& pxclr = pixels.at(x + (y * width));
					if (pxclr != bg)
					{
						fg = pxclr;
						chunkset |= 0b1000;
					}
				}
				{
					const rgb& pxclr = pixels.at(x + 1 + (y * width));
					if (pxclr != bg)
					{
						fg = pxclr;
						chunkset |= 0b0100;
					}
				}
				{
					const rgb& pxclr = pixels.at(x + ((y + 1) * width));
					if (pxclr != bg)
					{
						fg = pxclr;
						chunkset |= 0b0010;
					}
				}
				{
					const rgb& pxclr = pixels.at(x + 1 + ((y + 1) * width));
					if (pxclr != bg)
					{
						fg = pxclr;
						chunkset |= 0b0001;
					}
				}
				str.append(console.strSetBackgroundColour<std::u16string>(bg.r, bg.g, bg.b));
				str.append(console.strSetForegroundColour<std::u16string>(fg.r, fg.g, fg.b));
				str.push_back(chunkToChar(chunkset));
			}
		}
		return str;
	}

	char16_t canvas::chunkToChar(uint8_t chunkset) noexcept
	{
		switch (chunkset)
		{
			// 1 px
		case 0b1000: return u'\u2598';
		case 0b0100: return u'\u259D';
		case 0b0010: return u'\u2596';
		case 0b0001: return u'\u2597';
			// 2 px, sides
		case 0b1100: return u'\u2580';
		case 0b0011: return u'\u2584';
		case 0b1010: return u'\u258C';
		case 0b0101: return u'\u2590';
			// 2 px, corners
		case 0b1001: return u'\u259A';
		case 0b0110: return u'\u259E';
			// 3 px
		case 0b0111: return u'\u259F';
		case 0b1011: return u'\u2599';
		case 0b1101: return u'\u259C';
		case 0b1110: return u'\u259B';
			// 4 px
		case 0b1111: return u'\u2588';
		}
		// 0 px
		return ' ';
	}

	void canvas::ensureWidthAndHeightAreEven()
	{
		auto even_width = ((width & 1) ? (width + 1) : width);
		auto even_height = ((height & 1) ? (height + 1) : height);
		if (width != even_width)
		{
			resizeWidth(even_width);
		}
		if (height != even_height)
		{
			pixels.resize(even_width * even_height);
			height = even_height;
		}
	}

	void canvas::resizeWidth(int new_width)
	{
		std::vector<rgb> new_pixels{};
		new_pixels.resize(new_width * height);
		for (size_t y = 0; y < height; ++y)
		{
			for (size_t x = 0; x < width; ++x)
			{
				new_pixels.at(x + (y * new_width)) = pixels.at(x + (y * width));
			}
		}
		width = new_width;
		pixels = std::move(new_pixels);
	}
}
