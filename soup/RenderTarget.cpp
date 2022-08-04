#include "RenderTarget.hpp"

#include <map>

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
}
