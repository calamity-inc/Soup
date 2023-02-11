#include "BCanvas.hpp"

#include <stack>

#include "base.hpp"

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
			set(lerp(a.x, b.x, t), lerp(a.y, b.y, t));
		}
	}

	static Vector2 quadraticBezier(const Vector2& p0, const Vector2& p1, const Vector2& p2, float t)
	{
		const float tinv = (1.0f - t);
		const float t2 = (t * t);
		const float tinv2 = (tinv * tinv);
		return Vector2(
			tinv2 * p0.x + 2 * tinv * t * p1.x + t2 * p2.x,
			tinv2 * p0.y + 2 * tinv * t * p1.y + t2 * p2.y
		);
	}

	void BCanvas::addQuadraticBezier(const Vector2& a, const Vector2& b, const Vector2& c)
	{
		for (float t = 0.0f; t < 1.0f; t += (0.4f / a.distance(c)))
		{
			auto p = quadraticBezier(a, b, c, t);
			set(p.x, p.y);
		}
	}

	static Vector2 cubicBezier(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3, float t)
	{
		const float tinv = (1.0f - t);
		const float t2 = (t * t);
		const float t3 = (t2 * t);
		const float tinv2 = (tinv * tinv);
		const float tinv3 = (tinv2 * tinv);
		return Vector2(
			tinv3 * p0.x + 3 * tinv2 * t * p1.x + 3 * tinv * t2 * p2.x + t3 * p3.x,
			tinv3 * p0.y + 3 * tinv2 * t * p1.y + 3 * tinv * t2 * p2.y + t3 * p3.y
		);
	}

	void BCanvas::addCubicBezier(const Vector2& a, const Vector2& b, const Vector2& c, const Vector2& d)
	{
		for (float t = 0.0f; t < 1.0f; t += (0.4f / a.distance(d)))
		{
			auto p = cubicBezier(a, b, c, d, t);
			set(p.x, p.y);
		}
	}

	// Not perfect.
	bool BCanvas::isPointInsideOfShape(unsigned int x, unsigned int y) const
	{
		if (get(x, y))
		{
			return false;
		}
		unsigned int intersections = 0;
		unsigned int intersection_buffer = 0;
		int dx = 1, dy = 1;
		if (x < (width / 2)) { dx = -1; }
		if (y < (height / 2)) { dy = -1; }
		while (true)
		{
			if (intersection_buffer != 0)
			{
				--intersection_buffer;
			}
			const bool can_go_x = ((unsigned int)(x + dx) < width);
			const bool can_go_y = ((unsigned int)(y + dy) < height);
			if (can_go_x)
			{
				if (get(x + dx, y))
				{
					if (intersection_buffer == 0)
					{
						++intersections;
						intersection_buffer = 2;
					}
				}
			}
			if (can_go_y)
			{
				if (get(x, y + dy))
				{
					if (intersection_buffer == 0)
					{
						++intersections;
						intersection_buffer = 2;
					}
				}
			}
			if (!can_go_x && !can_go_y)
			{
				break;
			}
			x += (can_go_x * dx);
			y += (can_go_y * dy);
			if (get(x, y))
			{
				if (intersection_buffer == 0)
				{
					++intersections;
					intersection_buffer = 2;
				}
			}
		}
		return intersections % 2 == 1;
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
