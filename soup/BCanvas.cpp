#include "BCanvas.hpp"

#include <stack>

#include "base.hpp"

#include "visCurves.hpp"
#include "Vector2.hpp"

namespace soup
{
	void BCanvas::set(unsigned int x, unsigned int y, bool b) noexcept
	{
		SOUP_IF_LIKELY (x < width && y < height)
		{
			pixels.at(x + (y * width)) = b;
		}
	}

	bool BCanvas::get(unsigned int x, unsigned int y) const
	{
		return pixels.at(x + (y * width));
	}

	void BCanvas::addLine(const Vector2& a, const Vector2& b)
	{
		for (float t = 0.0f; t < 1.0f; t += (0.5f / a.distance(b)))
		{
			set(static_cast<unsigned int>(lerp(a.x, b.x, t)), static_cast<unsigned int>(lerp(a.y, b.y, t)));
		}
	}

	void BCanvas::addQuadraticBezier(const Vector2& a, const Vector2& b, const Vector2& c)
	{
		for (float t = 0.0f; t < 1.0f; t += (0.4f / a.distance(c)))
		{
			auto p = visCurves::quadraticBezier(a, b, c, t);
			set(static_cast<unsigned int>(p.x), static_cast<unsigned int>(p.y));
		}
	}

	void BCanvas::addCubicBezier(const Vector2& a, const Vector2& b, const Vector2& c, const Vector2& d)
	{
		for (float t = 0.0f; t < 1.0f; t += (0.4f / a.distance(d)))
		{
			auto p = visCurves::cubicBezier(a, b, c, d, t);
			set(static_cast<unsigned int>(p.x), static_cast<unsigned int>(p.y));
		}
	}

	void BCanvas::floodFill(unsigned int x, unsigned int y)
	{
		SOUP_IF_UNLIKELY (get(x, y))
		{
			return;
		}
		set(x, y);

		std::stack<std::pair<unsigned int, unsigned int>> stk;
		while (true)
		{
			if (x != 0 && !get(x - 1, y)) { set(x - 1, y); stk.push({ x - 1, y }); }
			if (y != 0 && !get(x, y - 1)) { set(x, y - 1); stk.push({ x, y - 1 }); }
			if (x + 1 != width && !get(x + 1, y)) { set(x + 1, y); stk.push({ x + 1, y }); }
			if (y + 1 != height && !get(x, y + 1)) { set(x, y + 1); stk.push({ x, y + 1 }); }

			SOUP_IF_UNLIKELY (stk.empty())
			{
				return;
			}

			x = stk.top().first;
			y = stk.top().second;
			stk.pop();
		}
	}
}
