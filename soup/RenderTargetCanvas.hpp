#pragma once

#include "RenderTarget.hpp"

namespace soup
{
	struct RenderTargetCanvas : public RenderTarget
	{
		Canvas* canvas;

		RenderTargetCanvas(Canvas* canvas);

		void drawRect(size_t x, size_t y, size_t width, size_t height, Rgb colour) final;
	};
}
