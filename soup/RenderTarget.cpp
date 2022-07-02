#include "RenderTarget.hpp"

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

	void RenderTarget::drawCircle(size_t x, size_t y, float r, Rgb colour)
	{
		for (float i = 0.01f; i < M_TAU; i += 0.01f)
		{
			drawRect(x + (size_t)(cos(i) * r), y + (size_t)(sin(i) * r), 1, 1, colour);
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

	void RenderTarget::drawText(size_t x, size_t y, const std::string& text, const RasterFont& font, Rgb colour, uint8_t scale)
	{
		drawText(x, y, unicode::utf8_to_utf32(text), font, colour, scale);
	}

	void RenderTarget::drawText(size_t x, size_t y, const std::u32string& text, const RasterFont& font, Rgb colour, uint8_t scale)
	{
		size_t text_x = (x / scale);
		for (const auto& c : text)
		{
			const auto& g = font.get(c);
			size_t i = 0;
			for (size_t glyph_y = 0; glyph_y != g.height; ++glyph_y)
			{
				for (size_t glyph_x = 0; glyph_x != g.width; ++glyph_x)
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
