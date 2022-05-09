#pragma once

#include "RenderTarget.hpp"

#include <Windows.h>

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
	};
}
