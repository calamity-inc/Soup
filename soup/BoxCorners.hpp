#pragma once

#include "fwd.hpp"

#include <array>

#include "Vector3.hpp"

namespace soup
{
#pragma pack(push, 1)
#pragma warning(push)
#pragma warning(disable: 26495) // uninitialised member variable
	struct BoxCorners
	{
		Vector3 back_lower_left;
		Vector3 edge2; // front lower left
		Vector3 edge3; // front upper left
		Vector3 edge4; // back upper left
		Vector3 front_upper_right;
		Vector3 edge6; // back upper right
		Vector3 edge7; // back lower right
		Vector3 edge8; // front lower right

		explicit BoxCorners() noexcept = default;

		explicit BoxCorners(const Vector3& origin, const Vector3& extent, const Vector3& right, const Vector3& forward, const Vector3& up) noexcept;

		[[nodiscard]] std::array<Poly, 12> toPolys() const noexcept;

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
