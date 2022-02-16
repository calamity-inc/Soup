#include "poly.hpp"

#include "ray.hpp"

namespace soup
{
	bool poly::checkRayIntersection(const ray& r, vector3& outHit) const
	{
		// Find Triangle Normal
		auto Normal = (b - a).crossProduct(c - a);
		Normal.normalize(); // not really needed

		// Find distance from LP1 and LP2 to the plane defined by the triangle
		float Dist1 = (r.start - a).dot(Normal);
		float Dist2 = (r.end - a).dot(Normal);
		if ((Dist1 * Dist2) >= 0.0f) return false;  // line doesn't cross the triangle.
		if (Dist1 == Dist2) return false;// line and plane are parallel

		// Find point on the line that intersects with the plane
		outHit = r.start + (r.end - r.start) * (-Dist1 / (Dist2 - Dist1));

		// Find if the interesection point lies inside the triangle by testing it against all edges
		vector3 vTest;
		vTest = Normal.crossProduct(b - a);
		if (vTest.dot(outHit - a) < 0.0f) return false;
		vTest = Normal.crossProduct(c - b);
		if (vTest.dot(outHit - b) < 0.0f) return false;
		vTest = Normal.crossProduct(a - c);
		if (vTest.dot(outHit - a) < 0.0f) return false;

		return true;
	}
}
