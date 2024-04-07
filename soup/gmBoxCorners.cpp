#include "gmBoxCorners.hpp"

#include "Poly.hpp"

NAMESPACE_SOUP
{
	gmBoxCorners::gmBoxCorners(const Vector3& origin, const Vector3& extent, const Vector3& right, const Vector3& forward, const Vector3& up) noexcept
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

	std::array<Poly, 12> gmBoxCorners::toPolys() const noexcept
	{
		return std::array<Poly, 12>{
			// front
			Poly{ edge2, front_upper_right, edge3 },
			Poly{ front_upper_right, edge2, edge8 },
			// top
			Poly{ front_upper_right, edge6, edge4 },
			Poly{ edge3, front_upper_right, edge4 },
			// right (from front view)
			Poly{ edge6, front_upper_right, edge7 },
			Poly{ edge7, front_upper_right, edge8 },
			// left (from front view)
			Poly{ back_lower_left, edge2, edge4 },
			Poly{ edge2, edge3, edge4 },
			// bottom
			Poly{ back_lower_left, edge7, edge8 },
			Poly{ edge2, back_lower_left, edge8 },
			// back
			Poly{ back_lower_left, edge4, edge6 },
			Poly{ back_lower_left, edge6, edge7 }
		};
	}
}
