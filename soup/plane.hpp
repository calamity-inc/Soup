#pragma once

#include "poly.hpp"
#include "vector3.hpp"

namespace soup
{
	struct plane
	{
		vector3 a;
		vector3 b;

		[[nodiscard]] std::array<poly, 2> toPolys() const
		{
			vector3 top_left = a;
			vector3 top_right = { a.x, b.y, a.z };
			vector3 bottom_left = { b.x, a.y, b.z };
			vector3 bottom_right = b;
			return std::array<poly, 2>{
				poly{
					top_right,
					top_left,
					bottom_left
				},
				poly{
					bottom_right,
					top_right,
					bottom_left,
				}
			};
		}
	};
}
