#include "Layout.hpp"

#include "RasterFont.hpp"
#include "Rgb.hpp"
#include "RenderTarget.hpp"

namespace soup
{
	void Layout::draw(RenderTarget& rt)
	{
		auto font = RasterFont::simple8();

		size_t x = 0;
		size_t y = 0;
		for (const auto& block : blocks)
		{
			x += block.margin;
			y += block.margin;
			for (const auto& elm : block.elms)
			{
				rt.drawText(x, y, elm.text, font, Rgb::WHITE, elm.font_size / font.baseline_glyph_height);
				x += (font.measureWidth(elm.text) + 1);
				x += (font.get(' ').width + 1);
			}
			x -= block.margin;
			y += (font.baseline_glyph_height * 2);
		}
	}
}
