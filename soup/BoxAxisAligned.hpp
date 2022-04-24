#pragma once

#include "Vector3.hpp"

namespace soup
{
#pragma pack(push, 1)
	struct BoxAxisAligned
	{
		Vector3 back_lower_left;
		Vector3 front_upper_right;

		[[nodiscard]] bool checkLineIntersection(const Vector3& L1, const Vector3& L2, Vector3& Hit) noexcept;
	};
#pragma pack(pop)
}
