#pragma once

#include "Vector2.hpp"

NAMESPACE_SOUP
{
	struct visCurves
	{
		[[nodiscard]] static Vector2 quadraticBezier(const Vector2& p0, const Vector2& p1, const Vector2& p2, float t)
		{
			const float tinv = (1.0f - t);
			const float t2 = (t * t);
			const float tinv2 = (tinv * tinv);
			return Vector2(
				tinv2 * p0.x + 2 * tinv * t * p1.x + t2 * p2.x,
				tinv2 * p0.y + 2 * tinv * t * p1.y + t2 * p2.y
			);
		}

		[[nodiscard]] static Vector2 cubicBezier(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Vector2& p3, float t)
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
	};
}
