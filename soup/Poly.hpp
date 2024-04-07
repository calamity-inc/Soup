#pragma once

#include "fwd.hpp"

#include "Vector3.hpp"

NAMESPACE_SOUP
{
#pragma pack(push, 1)
	struct Poly
	{
		Vector3 a;
		Vector3 b;
		Vector3 c;

		[[nodiscard]] bool operator==(const Poly& b) const;
		[[nodiscard]] bool operator!=(const Poly& b) const;

		bool checkRayIntersection(const Ray& r, Vector3& outHit) const;

		[[nodiscard]] Vector3 getSurfaceNormal() const;
		[[nodiscard]] Vector3 getCentrePoint() const;
		[[nodiscard]] const Vector3& getClosestPoint(const Vector3& pos) const;

		[[nodiscard]] float distance(const Vector3& pos) const;
	};
#pragma pack(pop)
}
