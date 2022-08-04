#pragma once

#include "fwd.hpp"

#include <string>

namespace soup
{
	struct RenderTarget
	{
		unsigned int width;
		unsigned int height;

		RenderTarget(unsigned int width, unsigned int height)
			: width(width), height(height)
		{
		}

		void fill(Rgb colour);

		void drawPixel(unsigned int x, unsigned int y, Rgb colour);

		virtual void drawRect(unsigned int x, unsigned int y, unsigned int width, unsigned int height, Rgb colour) = 0;
		void drawHollowCircle(unsigned int x, unsigned int y, float r, Rgb colour);

		virtual void drawLine(Vector2 a, Vector2 b, Rgb colour);
		void drawLine(Vector2 a, Vector2 b, Rgb a_colour, Rgb b_colour);

		void drawTriangle(Vector2 a, Vector2 b, Vector2 c, Rgb colour);
		void drawHollowTriangle(Vector2 a, Vector2 b, Vector2 c, Rgb colour);

		void drawText(unsigned int x, unsigned int y, const std::string& text, const RasterFont& font, Rgb colour, uint8_t scale = 1);
		void drawText(unsigned int x, unsigned int y, const std::u32string& text, const RasterFont& font, Rgb colour, uint8_t scale = 1);

		[[nodiscard]] virtual Rgb getPixel(unsigned int x, unsigned int y) const = 0;
	};
}
