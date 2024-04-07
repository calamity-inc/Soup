#pragma once

#include "Vector3.hpp"

NAMESPACE_SOUP
{
#pragma pack(push, 1)
	struct gmAxisAlignedBox
	{
		Vector3 back_lower_left;
		Vector3 front_upper_right;

		[[nodiscard]] bool checkLineIntersection(const Vector3& L1, const Vector3& L2, Vector3& Hit) noexcept;
	};
#pragma pack(pop)
}
