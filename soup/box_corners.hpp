#pragma once

#include "fwd.hpp"

#include <array>

#include "vector3.hpp"

namespace soup
{
#pragma pack(push, 1)
#pragma warning(push)
#pragma warning(disable: 26495) // uninitialised member variable
	struct box_corners
	{
		vector3 back_lower_left;
		vector3 edge2; // front lower left
		vector3 edge3; // front upper left
		vector3 edge4; // back upper left
		vector3 front_upper_right;
		vector3 edge6; // back upper right
		vector3 edge7; // back lower right
		vector3 edge8; // front lower right

		explicit box_corners() noexcept = default;

		explicit box_corners(const vector3& origin, const vector3& extent, const vector3& right, const vector3& forward, const vector3& up) noexcept;

		[[nodiscard]] std::array<poly, 12> toPolys() const noexcept;

		/**
		 * draw_line(back_lower_left, edge2);
		 * draw_line(back_lower_left, edge4);
		 * draw_line(edge2, edge3);
		 * draw_line(edge3, edge4);
		 * draw_line(front_upper_right, edge6);
		 * draw_line(front_upper_right, edge8);
		 * draw_line(edge6, edge7);
		 * draw_line(edge7, edge8);
		 * draw_line(back_lower_left, edge7);
		 * draw_line(edge2, edge8);
		 * draw_line(edge3, front_upper_right);
		 * draw_line(edge4, edge6);
		*/
	};
#pragma warning(pop)
#pragma pack(pop)
}
