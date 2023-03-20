#pragma once

#include "RenderTarget.hpp"

#include <Windows.h>

#include "Rgb.hpp"
#include "Vector2.hpp"

namespace soup
{
	struct RenderTargetWindow : public RenderTarget
	{
		HDC hdc;

		RenderTargetWindow(LONG width, LONG height, HDC hdc)
			: RenderTarget(width, height), hdc(hdc)
		{
		}

		void drawRect(unsigned int x, unsigned int y, unsigned int width, unsigned int height, Rgb colour) final
		{
			HBRUSH brush = CreateSolidBrush(RGB(colour.r, colour.g, colour.b));

			RECT r;
			r.left = x;
			r.top = y;
			r.right = (x + width);
			r.bottom = (y + height);
			FillRect(hdc, &r, brush);

			DeleteObject(brush);
		}

		void drawLine(Vector2 a, Vector2 b, Rgb colour) final
		{
			SelectObject(hdc, GetStockObject(DC_PEN));
			SetDCPenColor(hdc, RGB(colour.r, colour.g, colour.b));

			MoveToEx(hdc, a.x, a.y, nullptr);
			LineTo(hdc, b.x, b.y);
		}

		void drawCircle(unsigned int x, unsigned int y, float r, Rgb colour) final
		{
			return drawEllipse(x, y, r, r, colour);
		}

		void drawEllipse(unsigned int x, unsigned int y, float xr, float yr, Rgb colour) final
		{
			// Outline
			SelectObject(hdc, GetStockObject(DC_PEN));
			SetDCPenColor(hdc, RGB(colour.r, colour.g, colour.b));

			// Filling
			SelectObject(hdc, GetStockObject(DC_BRUSH));
			SetDCBrushColor(hdc, RGB(colour.r, colour.g, colour.b));

			Ellipse(hdc, x - xr, y - yr, x + xr, y + yr);
		}

		[[nodiscard]] Rgb getPixel(unsigned int x, unsigned int y) const final
		{
			auto col = GetPixel(hdc, x, y);
			return Rgb{ GetRValue(col), GetGValue(col), GetBValue(col) };
		}
	};
}
