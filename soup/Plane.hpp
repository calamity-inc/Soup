#pragma once

#include "Poly.hpp"
#include "Vector3.hpp"

namespace soup
{
	struct Plane
	{
		Vector3 a;
		Vector3 b;

		[[nodiscard]] std::array<Poly, 2> toPolys() const
		{
			Vector3 top_left = a;
			Vector3 top_right = { a.x, b.y, a.z };
			Vector3 bottom_left = { b.x, a.y, b.z };
			Vector3 bottom_right = b;
			return std::array<Poly, 2>{
				Poly{
					top_right,
					top_left,
					bottom_left
				},
				Poly{
					bottom_right,
					top_right,
					bottom_left,
				}
			};
		}
	};
}
