#include "canvas.hpp"

#include "console.hpp"
#include "string.hpp"

namespace soup
{
	void canvas::resize(int width, int height)
	{
		pixels.resize((size_t)width * height);
	}

	void canvas::fill(const rgb colour)
	{
		for (auto& p : pixels)
		{
			p = colour;
		}
	}

	void canvas::set(int x, int y, rgb colour)
	{
		pixels.at((size_t)x + (y * width)) = colour;
	}

	rgb canvas::get(int x, int y) const
	{
		return pixels.at((size_t)x + (y * width));
	}

	const rgb& canvas::ref(size_t x, size_t y) const
	{
		return pixels.at(x + (y * width));
	}

	std::string canvas::toString(bool explicit_nl) const
	{
		std::string str{};
		rgb prev = pixels.front();
		++prev.r;
		for (size_t y = 0; y != height; ++y)
		{
			for (size_t x = 0; x != width; ++x)
			{
				const rgb& colour = ref(x, y);
				if (colour != prev)
				{
					prev = colour;
					str.append(console.strSetForegroundColour<std::string>(colour.r, colour.g, colour.b));
					str.append(console.strSetBackgroundColour<std::string>(colour.r, colour.g, colour.b));
				}
				str.push_back('-');
			}
			if (explicit_nl)
			{
				str.push_back('\n');
			}
		}
		return str;
	}

	std::string canvas::toStringDoublewidth(bool explicit_nl) const
	{
		std::string str{};
		rgb prev = pixels.front();
		++prev.r;
		for (size_t y = 0; y != height; ++y)
		{
			for (size_t x = 0; x != width; ++x)
			{
				const rgb& colour = ref(x, y);
				if (colour != prev)
				{
					prev = colour;
					str.append(console.strSetForegroundColour<std::string>(colour.r, colour.g, colour.b));
					str.append(console.strSetBackgroundColour<std::string>(colour.r, colour.g, colour.b));
				}
				str.append("--");
			}
			if (explicit_nl)
			{
				str.push_back('\n');
			}
		}
		return str;
	}

	std::u16string canvas::toStringDownsampled(bool explicit_nl)
	{
		ensureWidthAndHeightAreEven();

		std::u16string str{};
		str.reserve((size_t)width * height);
		for (int y = 0; y != height; y += 2)
		{
			for (int x = 0; x != width; x += 2)
			{
				rgb bg = pixels.at(x + (y * width));
				rgb fg = bg;
				uint8_t chunkset = 0;
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
				str.push_back(downsampleChunkToChar(chunkset));
			}
			if (explicit_nl)
			{
				str.push_back(u'\n');
			}
		}
		return str;
	}

	std::u16string canvas::toStringDownsampledDoublewidth(bool explicit_nl)
	{
		ensureHeightIsEven();

		std::u16string str{};
		str.reserve((size_t)width * height);
		for (int y = 0; y != height; y += 2)
		{
			for (int x = 0; x != width; ++x)
			{
				rgb bg = pixels.at(x + (y * width));
				rgb fg = bg;
				uint8_t chunkset = 0;
				{
					const rgb& pxclr = pixels.at(x + (y * width));
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
					const rgb& pxclr = pixels.at(x + ((y + 1) * width));
					if (pxclr != bg)
					{
						fg = pxclr;
						chunkset |= 0b0001;
					}
				}
				str.append(console.strSetBackgroundColour<std::u16string>(bg.r, bg.g, bg.b));
				str.append(console.strSetForegroundColour<std::u16string>(fg.r, fg.g, fg.b));
				str.push_back(downsampleChunkToChar(chunkset));
			}
			if (explicit_nl)
			{
				str.push_back(u'\n');
			}
		}
		return str;
	}

	char16_t canvas::downsampleChunkToChar(uint8_t chunkset) noexcept
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
		return '-';
	}

	void canvas::ensureWidthAndHeightAreEven()
	{
		auto even_width = ((width & 1) ? (width + 1) : width);
		if (width != even_width)
		{
			resizeWidth(even_width);
		}

		ensureHeightIsEven();
	}

	void canvas::ensureHeightIsEven()
	{
		auto even_height = ((height & 1) ? (height + 1) : height);
		if (height != even_height)
		{
			pixels.resize(width * even_height);
			height = even_height;
		}
	}

	void canvas::resizeWidth(int new_width)
	{
		std::vector<rgb> new_pixels{};
		new_pixels.resize(new_width * height);
		for (size_t y = 0; y != height; ++y)
		{
			for (size_t x = 0; x != width; ++x)
			{
				new_pixels.at(x + (y * new_width)) = pixels.at(x + (y * width));
			}
		}
		width = new_width;
		pixels = std::move(new_pixels);
	}

	std::string canvas::toPPM() const
	{
		std::string res = "P3\n";
		res.append(string::decimal(width));
		res.push_back(' ');
		res.append(string::decimal(height));
		res.append("\n255\n");
		for (const auto& p : pixels)
		{
			res.append(string::decimal(p.r)).push_back(' ');
			res.append(string::decimal(p.g)).push_back(' ');
			res.append(string::decimal(p.b)).push_back('\n');
		}
		return res;
	}
}
