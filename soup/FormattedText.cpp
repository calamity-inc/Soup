#include "FormattedText.hpp"

#include "RenderTarget.hpp"
#include "RasterFont.hpp"

namespace soup
{
	std::pair<size_t, size_t> FormattedText::measure(const RasterFont& font) const
	{
		size_t width = 0;
		size_t height = 0;
		for (const auto& line : lines)
		{
			for (const auto& span : line)
			{
				width += font.measureWidth(span.text);
			}
			height += font.baseline_glyph_height;
		}
		return { width, height + 1 }; // heuristic to handle glyphs like ',' going beyond height
	}

	void FormattedText::draw(RenderTarget& rt, const RasterFont& font) const
	{
		size_t x = 0;
		size_t y = 0;
		for (const auto& line : lines)
		{
			for (const auto& span : line)
			{
				rt.drawText(x, y, span.text, font, span.colour);
				x += font.measureWidth(span.text);
			}
			y += font.baseline_glyph_height;
		}
	}
}
