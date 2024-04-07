#include "RenderTargetCanvas.hpp"

#include "Canvas.hpp"

NAMESPACE_SOUP
{
	RenderTargetCanvas::RenderTargetCanvas(Canvas& canvas)
		: RenderTarget{ canvas.width, canvas.height }, canvas(canvas)
	{
	}

	void RenderTargetCanvas::drawRect(unsigned int x, unsigned int y, unsigned int width, unsigned int height, Rgb colour)
	{
		canvas.addRect(x, y, width, height, colour);
	}

	Rgb RenderTargetCanvas::getPixel(unsigned int x, unsigned int y) const
	{
		return canvas.get(x, y);
	}
}
