#pragma once

#include "RenderTarget.hpp"

#include "base.hpp"
#include "Rgb.hpp"
#include "Vector2.hpp"

#if SOUP_WINDOWS
#include <Windows.h>
#pragma comment(lib, "Gdi32.lib")
#else
#include "X11Api.hpp"
#endif

namespace soup
{
	struct RenderTargetWindow : public RenderTarget
	{
#if SOUP_WINDOWS
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

			MoveToEx(hdc, static_cast<int>(a.x), static_cast<int>(a.y), nullptr);
			LineTo(hdc, static_cast<int>(b.x), static_cast<int>(b.y));
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

			Ellipse(hdc, static_cast<int>(x - xr), static_cast<int>(y - yr), static_cast<int>(x + xr), static_cast<int>(y + yr));
		}

		[[nodiscard]] Rgb getPixel(unsigned int x, unsigned int y) const final
		{
			auto col = GetPixel(hdc, x, y);
			return Rgb{ GetRValue(col), GetGValue(col), GetBValue(col) };
		}
#else
		X11Api::Window w;
		X11Api::GC gc;

		RenderTargetWindow(X11Api::Window w, X11Api::GC gc)
			// TODO: Pass on Window's width & height
			: RenderTarget(-1, -1), w(w), gc(gc)
		{
		}

		void drawRect(unsigned int x, unsigned int y, unsigned int width, unsigned int height, Rgb colour) final
		{
			const auto& xapi = X11Api::get();
			xapi.setForeground(xapi.display, gc, colour.toInt());
			xapi.fillRectangle(xapi.display, w, gc, x, y, width, height);
		}
#endif
	};
}
