#pragma once

#include "fwd.hpp"

#include "vector3.hpp"

namespace soup
{
#pragma pack(push, 1)
	struct poly
	{
		vector3 a;
		vector3 b;
		vector3 c;

		bool checkRayIntersection(const ray& r, vector3& outHit) const;
	};
#pragma pack(pop)
}
