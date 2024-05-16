#pragma once

#include "RenderTarget.hpp"

NAMESPACE_SOUP
{
	struct RenderTargetCanvas : public RenderTarget
	{
		Canvas& canvas;

		RenderTargetCanvas(Canvas& canvas);

		void drawRect(int x, int y, unsigned int width, unsigned int height, Rgb colour) final;

		[[nodiscard]] Rgb getPixel(int x, int y) const final;
	};
}
