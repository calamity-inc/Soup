#include "Canvas.hpp"

#include "console.hpp"
#include "ioSeekableReader.hpp"
#include "RasterFont.hpp"
#include "StringWriter.hpp"
#include "TinyPngOut.hpp"
#include "unicode.hpp"

namespace soup
{
	void Canvas::fill(const Rgb colour)
	{
		for (auto& p : pixels)
		{
			p = colour;
		}
	}

	void Canvas::loadBlackWhiteData(const std::vector<bool>& black_white_data)
	{
		unsigned int i = 0;
		for (const auto& px : black_white_data)
		{
			auto c = (uint8_t)(px * 255);
			pixels.at(i++) = Rgb{ c, c, c };
		}
	}

	void Canvas::set(unsigned int x, unsigned int y, Rgb colour) noexcept
	{
		if (x < width && y < height)
		{
			pixels.at(x + (y * width)) = colour;
		}
	}

	Rgb Canvas::get(unsigned int x, unsigned int y) const
	{
		return pixels.at(x + (y * width));
	}

	const Rgb& Canvas::ref(unsigned int x, unsigned int y) const
	{
		return pixels.at(x + (y * width));
	}

	void Canvas::addText(unsigned int x, unsigned int y, const std::string& text, const RasterFont& font)
	{
		return addText(x, y, unicode::utf8_to_utf32(text), font);
	}

	void Canvas::addText(unsigned int x, unsigned int y, const std::u32string& text, const RasterFont& font)
	{
		for (const auto& c : text)
		{
			const auto& g = font.get(c);
			addCanvas(x, y + g.y_offset, g.getCanvas());
			x += (g.width + 1);
		}
	}

	void Canvas::addCanvas(unsigned int x_offset, unsigned int y_offset, const Canvas& b)
	{
		for (unsigned int y = 0; y != b.height; ++y)
		{
			for (unsigned int x = 0; x != b.width; ++x)
			{
				set(x + x_offset, y + y_offset, b.get(x, y));
			}
		}
	}

	void Canvas::addRect(unsigned int x_offset, unsigned int y_offset, unsigned int width, unsigned int height, Rgb colour)
	{
		for (unsigned int y = 0; y != height; ++y)
		{
			for (unsigned int x = 0; x != width; ++x)
			{
				set(x + x_offset, y + y_offset, colour);
			}
		}
	}

	void Canvas::resize(unsigned int width, unsigned int height)
	{
		this->width = width;
		this->height = height;
		pixels.resize(width * height);
	}

	void Canvas::resizeWidth(unsigned int new_width)
	{
		std::vector<Rgb> new_pixels{};
		new_pixels.resize(new_width * height);
		for (unsigned int y = 0; y != height; ++y)
		{
			for (unsigned int x = 0; x != width; ++x)
			{
				new_pixels.at(x + (y * new_width)) = pixels.at(x + (y * width));
			}
		}
		width = new_width;
		pixels = std::move(new_pixels);
	}

	void Canvas::ensureWidthAndHeightAreEven()
	{
		auto even_width = ((width & 1) ? (width + 1) : width);
		if (width != even_width)
		{
			resizeWidth(even_width);
		}

		ensureHeightIsEven();
	}

	void Canvas::ensureHeightIsEven()
	{
		auto even_height = ((height & 1) ? (height + 1) : height);
		if (height != even_height)
		{
			pixels.resize(width * even_height);
			height = even_height;
		}
	}

	void Canvas::resizeNearestNeighbour(unsigned int desired_width, unsigned int desired_height)
	{
		Canvas c{ desired_width, desired_height };
		for (unsigned int y = 0; y != c.height; ++y)
		{
			for (unsigned int x = 0; x != c.width; ++x)
			{
				c.set(x, y, get(
					(unsigned int)(((double)x / c.width) * width),
					(unsigned int)(((double)y / c.height) * height)
				));
			}
		}
		*this = std::move(c);
	}

	std::string Canvas::toString(bool explicit_nl) const
	{
		std::string str{};
		Rgb prev = pixels.front();
		++prev.r;
		for (unsigned int y = 0; y != height; ++y)
		{
			for (unsigned int x = 0; x != width; ++x)
			{
				const Rgb& colour = ref(x, y);
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

	std::string Canvas::toStringDoublewidth(bool explicit_nl) const
	{
		std::string str{};
		Rgb prev = pixels.front();
		++prev.r;
		for (unsigned int y = 0; y != height; ++y)
		{
			for (unsigned int x = 0; x != width; ++x)
			{
				const Rgb& colour = ref(x, y);
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
				str.append(console.strResetColour<std::string>());
				str.push_back('\n');
			}
		}
		return str;
	}

	std::u16string Canvas::toStringDownsampled(bool explicit_nl)
	{
		ensureWidthAndHeightAreEven();

		std::u16string str{};
		str.reserve((unsigned int)width * height);
		for (int y = 0; y != height; y += 2)
		{
			for (int x = 0; x != width; x += 2)
			{
				Rgb bg = pixels.at(x + (y * width));
				Rgb fg = bg;
				uint8_t chunkset = 0;
				{
					const Rgb& pxclr = pixels.at(x + 1 + (y * width));
					if (pxclr != bg)
					{
						fg = pxclr;
						chunkset |= 0b0100;
					}
				}
				{
					const Rgb& pxclr = pixels.at(x + ((y + 1) * width));
					if (pxclr != bg)
					{
						fg = pxclr;
						chunkset |= 0b0010;
					}
				}
				{
					const Rgb& pxclr = pixels.at(x + 1 + ((y + 1) * width));
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

	std::u16string Canvas::toStringDownsampledDoublewidth(bool explicit_nl)
	{
		ensureHeightIsEven();

		std::u16string str{};
		str.reserve((unsigned int)width * height);
		for (int y = 0; y != height; y += 2)
		{
			for (int x = 0; x != width; ++x)
			{
				Rgb bg = pixels.at(x + (y * width));
				Rgb fg = bg;
				uint8_t chunkset = 0;
				{
					const Rgb& pxclr = pixels.at(x + (y * width));
					if (pxclr != bg)
					{
						fg = pxclr;
						chunkset |= 0b0100;
					}
				}
				{
					const Rgb& pxclr = pixels.at(x + ((y + 1) * width));
					if (pxclr != bg)
					{
						fg = pxclr;
						chunkset |= 0b0010;
					}
				}
				{
					const Rgb& pxclr = pixels.at(x + ((y + 1) * width));
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
				str.append(console.strResetColour<std::u16string>());
				str.push_back(u'\n');
			}
		}
		return str;
	}

	char16_t Canvas::downsampleChunkToChar(uint8_t chunkset) noexcept
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


	Canvas Canvas::fromBmp(ioSeekableReader& r)
	{
		Canvas c;

		uint16_t sig;
		SOUP_IF_LIKELY (r.u16(sig) && sig == 0x4D42)
		{
			uint32_t data_start, header_size;
			int32_t width, height;
			SOUP_IF_LIKELY ((r.seek(0x0A), r.u32(data_start))
				&& r.u32(header_size)
				&& header_size == 40
				&& r.i32(width)
				&& r.i32(height)
				&& height < 0
				)
			{
				height *= -1;

				c.resize(width, height);
				r.seek(data_start);

				for (auto& p : c.pixels)
				{
					SOUP_IF_UNLIKELY (!r.u8(p.r)
						|| !r.u8(p.g)
						|| !r.u8(p.b)
						)
					{
						break;
					}
				}
			}
		}

		return c;
	}

	std::string Canvas::toSvg(unsigned int scale) const
	{
		std::string str = R"(<svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink" width=")";
		str.append(std::to_string(width * scale));
		str.append(R"(" height=")");
		str.append(std::to_string(width * scale));
		str.append(R"(">)");

		std::string rect_suffix = R"(" width=")";
		rect_suffix.append(std::to_string(scale));
		rect_suffix.append(R"(" height=")");
		rect_suffix.append(std::to_string(scale));
		rect_suffix.append("\"/>");
		for (unsigned int y = 0; y != height; ++y)
		{
			for (unsigned int x = 0; x != width; ++x)
			{
				str.append(R"(<rect x=")");
				str.append(std::to_string(x * scale));
				str.append(R"(" y=")");
				str.append(std::to_string(y * scale));
				str.append(R"(" fill="#)");
				str.append(get(x, y).toHex());
				str.append(rect_suffix);
			}
		}
		str.append("</svg>");
		return str;
	}

	std::string Canvas::toPng() const
	{
		StringWriter sw;
		toPng(sw);
		return sw.str;
	}

	void Canvas::toPng(Writer& w) const
	{
		TinyPngOut po(width, height, w);
		po.write(pixels.data(), pixels.size());
	}

	std::string Canvas::toPpm() const
	{
		std::string res = "P3\n";
		res.append(std::to_string(width));
		res.push_back(' ');
		res.append(std::to_string(height));
		res.append("\n255\n");
		for (const auto& p : pixels)
		{
			res.append(std::to_string(p.r)).push_back(' ');
			res.append(std::to_string(p.g)).push_back(' ');
			res.append(std::to_string(p.b)).push_back('\n');
		}
		return res;
	}
}
