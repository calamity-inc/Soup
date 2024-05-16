#pragma once

#include "fwd.hpp"

#include <string>

NAMESPACE_SOUP
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

		void drawPixel(int x, int y, Rgb colour);

		virtual void drawRect(int x, int y, unsigned int width, unsigned int height, Rgb colour) = 0;
		void drawHollowRect(int x, int y, unsigned int width, unsigned int height, Rgb colour);

		void drawHollowCircle(int x, int y, float r, Rgb colour);
		virtual void drawCircle(int x, int y, float r, Rgb colour);
		virtual void drawEllipse(int x, int y, float xr, float yr, Rgb colour);

		virtual void drawLine(Vector2 a, Vector2 b, Rgb colour);
		void drawLine(Vector2 a, Vector2 b, Rgb a_colour, Rgb b_colour);

		void drawTriangle(Vector2 a, Vector2 b, Vector2 c, Rgb colour);
		void drawHollowTriangle(Vector2 a, Vector2 b, Vector2 c, Rgb colour);

		void drawText(int x, int y, const std::string& text, const RasterFont& font, Rgb colour, uint8_t scale = 1);
		void drawText(int x, int y, const std::u32string& text, const RasterFont& font, Rgb colour, uint8_t scale = 1);

		void drawCentredText(int x, int y, const std::string& text, const RasterFont& font, Rgb colour, uint8_t scale = 1);
		void drawCentredText(int x, int y, const std::u32string& text, const RasterFont& font, Rgb colour, uint8_t scale = 1);

		void drawRightboundText(int x, int y, const std::string& text, const RasterFont& font, Rgb colour, uint8_t scale = 1);
		void drawRightboundText(int x, int y, const std::u32string& text, const RasterFont& font, Rgb colour, uint8_t scale = 1);

		void drawCanvas(int x, int y, const Canvas& c);

		[[nodiscard]] virtual Rgb getPixel(int x, int y) const;
	};
}
