#pragma once

#include "RenderTarget.hpp"

NAMESPACE_SOUP
{
	struct RenderTargetCanvas : public RenderTarget
	{
		Canvas& canvas;

		RenderTargetCanvas(Canvas& canvas);

		void drawRect(unsigned int x, unsigned int y, unsigned int width, unsigned int height, Rgb colour) final;

		[[nodiscard]] Rgb getPixel(unsigned int x, unsigned int y) const final;
	};
}
