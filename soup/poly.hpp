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

		[[nodiscard]] bool operator==(const poly& b) const;
		[[nodiscard]] bool operator!=(const poly& b) const;

		bool checkRayIntersection(const ray& r, vector3& outHit) const;

		[[nodiscard]] vector3 getSurfaceNormal() const;
		[[nodiscard]] vector3 getCentrePoint() const;
		[[nodiscard]] const vector3& getClosestPoint(const vector3& pos) const;

		[[nodiscard]] float distance(const vector3& pos) const;
	};
#pragma pack(pop)
}
