#pragma once

#include "base.hpp"
#include "fwd.hpp"
#include "math.hpp" // lerp

#include <vector>

NAMESPACE_SOUP
{
	struct ResponseCurve
	{
		struct Point
		{
			float x, y;

			Point(float x, float y)
				: x(x), y(y)
			{
			}
		};

		std::vector<Point> points{}; // {0, 0} is implied to be first and {1, 1} is implied to be last. must be ordered by x ascending.

		[[nodiscard]] float getY(float x) const noexcept
		{
			SOUP_ASSUME(x >= 0.0f && x <= 1.0f);

			Point left{ 0.0f, 0.0f };
			Point right{ 1.0f, 1.0f };
			for (const auto& point : points)
			{
				if (x <= point.x)
				{
					right = point;
					break;
				}

				if (left.x <= point.x /*&& point.x < x*/)
				{
					left = point;
				}
			}

			return soup::lerp<float>(left.y, right.y, ((x - left.x) / (right.x - left.x)));
		}

		[[nodiscard]] Canvas toCanvas(unsigned int width, unsigned int height) const;
	};
}
