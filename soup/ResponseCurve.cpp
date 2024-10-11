#include "ResponseCurve.hpp"

#include "Canvas.hpp"

NAMESPACE_SOUP
{
	Canvas ResponseCurve::toCanvas(unsigned int width, unsigned int height) const
	{
		Canvas c(width, height);
		const float x_max = static_cast<float>(width - 1);
		const unsigned int y_max = height - 1;
		for (unsigned int x = 0; x != width; ++x)
		{
			const auto bar_height = static_cast<unsigned int>(this->getY(x / x_max) * y_max);
			c.addRect(x, y_max - bar_height, 1, bar_height, Rgb::WHITE);
		}
		return c;
	}
}
