#include "RenderTarget.hpp"

#include <map>

#include "Canvas.hpp"
#include "Exception.hpp"
#include "math.hpp"
#include "RasterFont.hpp"
#include "Rgb.hpp"
#include "unicode.hpp"
#include "Vector2.hpp"

namespace soup
{
	void RenderTarget::fill(Rgb colour)
	{
		drawRect(0, 0, width, height, colour);
	}

	void RenderTarget::drawPixel(unsigned int x, unsigned int y, Rgb colour)
	{
		drawRect(x, y, 1, 1, colour);
	}

	void RenderTarget::drawHollowCircle(unsigned int x, unsigned int y, float r, Rgb colour)
	{
		for (float i = 0.01f; i < M_TAU; i += 0.01f)
		{
			drawRect(x + (unsigned int)(cos(i) * r), y + (unsigned int)(sin(i) * r), 1, 1, colour);
		}
	}

	void RenderTarget::drawCircle(unsigned int x, unsigned int y, float r, Rgb colour)
	{
		drawEllipse(x, y, r, r, colour);
	}

	static bool isPointInsideEllipse(float x, float y, float xr, float yr, float px, float py)
	{
		float dx = (px - x) / xr;
		float dy = (py - y) / yr;
		return (dx * dx + dy * dy) < 1.0f;
	}

	void RenderTarget::drawEllipse(unsigned int x, unsigned int y, float xr, float yr, Rgb colour)
	{
		// This could be more efficient by only going over the rectangle [x-xr, y-yr, x+xr, y+yr]
		for (unsigned int px = 0; px != width; ++px)
		{
			for (unsigned int py = 0; py != height; ++py)
			{
				if (isPointInsideEllipse(x, y, xr, yr, px, py))
				{
					drawPixel(px, py, colour);
				}
			}
		}
	}

	void RenderTarget::drawLine(Vector2 a, Vector2 b, Rgb colour)
	{
		for (float t = 0.0f; t < 1.0f; t += (0.5f / a.distance(b)))
		{
			drawRect(lerp(a.x, b.x, t), lerp(a.y, b.y, t), 1, 1, colour);
		}
	}

	void RenderTarget::drawLine(Vector2 a, Vector2 b, Rgb a_colour, Rgb b_colour)
	{
		for (float t = 0.0f; t < 1.0f; t += (0.5f / a.distance(b)))
		{
			drawRect(lerp(a.x, b.x, t), lerp(a.y, b.y, t), 1, 1, Rgb::lerp(a_colour, b_colour, t));
		}
	}

	static void processPoint(std::map<unsigned int, std::pair<unsigned int, unsigned int>>& lines, unsigned int x, unsigned int y)
	{
		if (auto e = lines.find(y); e != lines.end())
		{
			if (x < e->second.first)
			{
				e->second.first = x;
			}
			else if (x > e->second.second)
			{
				e->second.second = x;
			}
		}
		else
		{
			lines.emplace(y, std::pair<unsigned int, unsigned int>{ x, x });
		}
	}

	static void processLine(std::map<unsigned int, std::pair<unsigned int, unsigned int>>& lines, Vector2 a, Vector2 b)
	{
		for (float t = 0.0f; t < 1.0f; t += (0.5f / a.distance(b)))
		{
			processPoint(lines, lerp(a.x, b.x, t), lerp(a.y, b.y, t));
		}
	}

	void RenderTarget::drawTriangle(Vector2 a, Vector2 b, Vector2 c, Rgb colour)
	{
		std::map<unsigned int, std::pair<unsigned int, unsigned int>> lines{};
		processLine(lines, a, b);
		processLine(lines, b, c);
		processLine(lines, c, a);
		for (const auto& line : lines)
		{
			drawRect(line.second.first, line.first, (line.second.second - line.second.first) + 1, 1, colour);
		}
	}

	void RenderTarget::drawHollowTriangle(Vector2 a, Vector2 b, Vector2 c, Rgb colour)
	{
		drawLine(a, b, colour);
		drawLine(b, c, colour);
		drawLine(c, a, colour);
	}

	void RenderTarget::drawText(unsigned int x, unsigned int y, const std::string& text, const RasterFont& font, Rgb colour, uint8_t scale)
	{
		drawText(x, y, unicode::utf8_to_utf32(text), font, colour, scale);
	}

	void RenderTarget::drawText(unsigned int x, unsigned int y, const std::u32string& text, const RasterFont& font, Rgb colour, uint8_t scale)
	{
		if (scale == 0)
		{
			return;
		}

		unsigned int text_x = (x / scale);
		for (const auto& c : text)
		{
			const auto& g = font.get(c);
			unsigned int i = 0;
			for (unsigned int glyph_y = 0; glyph_y != g.height; ++glyph_y)
			{
				for (unsigned int glyph_x = 0; glyph_x != g.width; ++glyph_x)
				{
					if (g.pixels.at(i))
					{
						drawRect((text_x + glyph_x) * scale, y + ((glyph_y + g.y_offset) * scale), scale, scale, colour);
					}
					++i;
				}
			}
			text_x += (g.width + 1);
		}
	}

	void RenderTarget::drawCentredText(unsigned int x, unsigned int y, const std::string& text, const RasterFont& font, Rgb colour, uint8_t scale)
	{
		drawCentredText(x, y, unicode::utf8_to_utf32(text), font, colour, scale);
	}

	void RenderTarget::drawCentredText(unsigned int x, unsigned int y, const std::u32string& text, const RasterFont& font, Rgb colour, uint8_t scale)
	{
		drawText(x - (font.measureWidth(text) * scale * 0.5f), y, text, font, colour, scale);
	}

	void RenderTarget::drawRightboundText(unsigned int x, unsigned int y, const std::string& text, const RasterFont& font, Rgb colour, uint8_t scale)
	{
		drawRightboundText(x, y, unicode::utf8_to_utf32(text), font, colour, scale);
	}

	void RenderTarget::drawRightboundText(unsigned int x, unsigned int y, const std::u32string& text, const RasterFont& font, Rgb colour, uint8_t scale)
	{
		drawText(x - (font.measureWidth(text) * scale), y, text, font, colour, scale);
	}

	void RenderTarget::drawCanvas(unsigned int x, unsigned int y, const Canvas& c)
	{
		for (unsigned int canvas_y = 0; canvas_y != c.height; ++canvas_y)
		{
			for (unsigned int canvas_x = 0; canvas_x != c.width; ++canvas_x)
			{
				drawPixel(x + canvas_x, y + canvas_y, c.get(canvas_x, canvas_y));
			}
		}
	}

	Rgb RenderTarget::getPixel(unsigned int x, unsigned int y) const
	{
		Exception::purecall();
	}
}
