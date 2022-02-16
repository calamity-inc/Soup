#include "box_axis_aligned.hpp"

namespace soup
{
	// From https://www.3dkingdoms.com/weekly/weekly.php?a=3

	bool inline GetIntersection(float fDst1, float fDst2, const vector3& P1, const vector3& P2, vector3& Hit)
	{
		if ((fDst1 * fDst2) >= 0.0f) return false;
		if (fDst1 == fDst2) return false;
		Hit = P1 + (P2 - P1) * (-fDst1 / (fDst2 - fDst1));
		return true;
	}

	bool inline InBox(const vector3& Hit, const vector3& B1, const vector3& B2, const int Axis)
	{
		if (Axis == 1 && Hit.z > B1.z && Hit.z < B2.z && Hit.y > B1.y && Hit.y < B2.y) return true;
		if (Axis == 2 && Hit.z > B1.z && Hit.z < B2.z && Hit.x > B1.x && Hit.x < B2.x) return true;
		if (Axis == 3 && Hit.x > B1.x && Hit.x < B2.x && Hit.y > B1.y && Hit.y < B2.y) return true;
		return false;
	}

	bool box_axis_aligned::checkLineIntersection(const vector3& L1, const vector3& L2, vector3& Hit) noexcept
	{
		if (L2.x < back_lower_left.x && L1.x < back_lower_left.x) return false;
		if (L2.x > front_upper_right.x && L1.x > front_upper_right.x) return false;
		if (L2.y < back_lower_left.y && L1.y < back_lower_left.y) return false;
		if (L2.y > front_upper_right.y && L1.y > front_upper_right.y) return false;
		if (L2.z < back_lower_left.z && L1.z < back_lower_left.z) return false;
		if (L2.z > front_upper_right.z && L1.z > front_upper_right.z) return false;
		if (L1.x > back_lower_left.x && L1.x < front_upper_right.x &&
			L1.y > back_lower_left.y && L1.y < front_upper_right.y &&
			L1.z > back_lower_left.z && L1.z < front_upper_right.z)
		{
			Hit = L1;
			return true;
		}
		if ((GetIntersection(L1.x - back_lower_left.x, L2.x - back_lower_left.x, L1, L2, Hit) && InBox(Hit, back_lower_left, front_upper_right, 1))
			|| (GetIntersection(L1.y - back_lower_left.y, L2.y - back_lower_left.y, L1, L2, Hit) && InBox(Hit, back_lower_left, front_upper_right, 2))
			|| (GetIntersection(L1.z - back_lower_left.z, L2.z - back_lower_left.z, L1, L2, Hit) && InBox(Hit, back_lower_left, front_upper_right, 3))
			|| (GetIntersection(L1.x - front_upper_right.x, L2.x - front_upper_right.x, L1, L2, Hit) && InBox(Hit, back_lower_left, front_upper_right, 1))
			|| (GetIntersection(L1.y - front_upper_right.y, L2.y - front_upper_right.y, L1, L2, Hit) && InBox(Hit, back_lower_left, front_upper_right, 2))
			|| (GetIntersection(L1.z - front_upper_right.z, L2.z - front_upper_right.z, L1, L2, Hit) && InBox(Hit, back_lower_left, front_upper_right, 3)))
			return true;

		return false;
	}
}
