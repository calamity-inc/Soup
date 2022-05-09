#include "RenderTargetCanvas.hpp"

#include "Canvas.hpp"

namespace soup
{
	RenderTargetCanvas::RenderTargetCanvas(Canvas* canvas)
		: RenderTarget{ canvas->width, canvas->height }, canvas(canvas)
	{
	}

	void RenderTargetCanvas::drawRect(size_t x, size_t y, size_t width, size_t height, Rgb colour)
	{
		canvas->addRect(x, y, width, height, colour);
	}
}
