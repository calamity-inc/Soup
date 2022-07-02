#pragma once

#include "fwd.hpp"

#include <string>

namespace soup
{
	struct RenderTarget
	{
		size_t width;
		size_t height;

		RenderTarget(size_t width, size_t height)
			: width(width), height(height)
		{
		}

		void fill(Rgb colour);

		virtual void drawRect(size_t x, size_t y, size_t width, size_t height, Rgb colour) = 0;
		void drawHollowCircle(size_t x, size_t y, float r, Rgb colour);

		virtual void drawLine(Vector2 a, Vector2 b, Rgb colour);
		void drawLine(Vector2 a, Vector2 b, Rgb a_colour, Rgb b_colour);

		void drawTriangle(Vector2 a, Vector2 b, Vector2 c, Rgb colour);
		void drawHollowTriangle(Vector2 a, Vector2 b, Vector2 c, Rgb colour);

		void drawText(size_t x, size_t y, const std::string& text, const RasterFont& font, Rgb colour, uint8_t scale = 1);
		void drawText(size_t x, size_t y, const std::u32string& text, const RasterFont& font, Rgb colour, uint8_t scale = 1);
	};
}
