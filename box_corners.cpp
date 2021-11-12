#include "box_corners.hpp"

#include "poly.hpp"

namespace soup
{
	box_corners::box_corners(const vector3& origin, const vector3& extent, const vector3& right, const vector3& forward, const vector3& up) noexcept
	{
		this->front_upper_right.x = origin.x + extent.y * right.x + extent.x * forward.x + extent.z * up.x;
		this->front_upper_right.y = origin.y + extent.y * right.y + extent.x * forward.y + extent.z * up.y;
		this->front_upper_right.z = origin.z + extent.y * right.z + extent.x * forward.z + extent.z * up.z;

		this->back_lower_left.x = origin.x - extent.y * right.x - extent.x * forward.x - extent.z * up.x;
		this->back_lower_left.y = origin.y - extent.y * right.y - extent.x * forward.y - extent.z * up.y;
		this->back_lower_left.z = origin.z - extent.y * right.z - extent.x * forward.z - extent.z * up.z;

		this->edge2.x = this->back_lower_left.x + 2 * extent.y * right.x;
		this->edge2.y = this->back_lower_left.y + 2 * extent.y * right.y;
		this->edge2.z = this->back_lower_left.z + 2 * extent.y * right.z;

		this->edge3.x = this->edge2.x + 2 * extent.z * up.x;
		this->edge3.y = this->edge2.y + 2 * extent.z * up.y;
		this->edge3.z = this->edge2.z + 2 * extent.z * up.z;

		this->edge4.x = this->back_lower_left.x + 2 * extent.z * up.x;
		this->edge4.y = this->back_lower_left.y + 2 * extent.z * up.y;
		this->edge4.z = this->back_lower_left.z + 2 * extent.z * up.z;

		this->edge6.x = this->front_upper_right.x - 2 * extent.y * right.x;
		this->edge6.y = this->front_upper_right.y - 2 * extent.y * right.y;
		this->edge6.z = this->front_upper_right.z - 2 * extent.y * right.z;

		this->edge7.x = this->edge6.x - 2 * extent.z * up.x;
		this->edge7.y = this->edge6.y - 2 * extent.z * up.y;
		this->edge7.z = this->edge6.z - 2 * extent.z * up.z;

		this->edge8.x = this->front_upper_right.x - 2 * extent.z * up.x;
		this->edge8.y = this->front_upper_right.y - 2 * extent.z * up.y;
		this->edge8.z = this->front_upper_right.z - 2 * extent.z * up.z;
	}

	std::array<poly, 12> box_corners::asPolys() const noexcept
	{
		return std::array<poly, 12>{
			// front
			poly{ edge2, edge3, front_upper_right },
			poly{ edge2, front_upper_right, edge8 },
			// top
			poly{ edge4, edge6, front_upper_right },
			poly{ front_upper_right, edge3, edge4 },
			// right
			poly{ front_upper_right, edge6, edge7 },
			poly{ front_upper_right, edge7, edge8 },
			// left
			poly{ back_lower_left, edge2, edge4 },
			poly{ edge2, edge3, edge4 },
			// bottom
			poly{ back_lower_left, edge7, edge8 },
			poly{ back_lower_left, edge2, edge8 },
			// back
			poly{ back_lower_left, edge4, edge6 },
			poly{ back_lower_left, edge6, edge7 }
		};
	}
}
