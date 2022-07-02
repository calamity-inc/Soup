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

		void drawRect(size_t x, size_t y, size_t width, size_t height, Rgb colour) final
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
	};
}
