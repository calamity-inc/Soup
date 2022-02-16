#pragma once

#include "vector3.hpp"

namespace soup
{
#pragma pack(push, 1)
	struct box_axis_aligned
	{
		vector3 back_lower_left;
		vector3 front_upper_right;

		[[nodiscard]] bool checkLineIntersection(const vector3& L1, const vector3& L2, vector3& Hit) noexcept;
	};
#pragma pack(pop)
}
