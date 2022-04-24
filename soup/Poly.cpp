#include "Poly.hpp"

#include <algorithm> // max

#include "Ray.hpp"

namespace soup
{
	bool Poly::operator==(const Poly& b) const
	{
		return this->a.eq(b.a)
			&& this->b.eq(b.b)
			&& this->c.eq(b.c)
			;
	}

	bool Poly::operator!=(const Poly& b) const
	{
		return !operator==(b);
	}

	bool Poly::checkRayIntersection(const Ray& r, Vector3& outHit) const
	{
		// Find Triangle Normal
		auto Normal = (b - a).crossProduct(c - a);
		Normal.normalise(); // not really needed

		// Find distance from LP1 and LP2 to the plane defined by the triangle
		float Dist1 = (r.start - a).dot(Normal);
		float Dist2 = (r.end - a).dot(Normal);
		if ((Dist1 * Dist2) >= 0.0f) return false;  // line doesn't cross the triangle.
		if (Dist1 == Dist2) return false;// line and plane are parallel

		// Find point on the line that intersects with the plane
		outHit = r.start + (r.end - r.start) * (-Dist1 / (Dist2 - Dist1));

		// Find if the interesection point lies inside the triangle by testing it against all edges
		Vector3 vTest;
		vTest = Normal.crossProduct(b - a);
		if (vTest.dot(outHit - a) < 0.0f) return false;
		vTest = Normal.crossProduct(c - b);
		if (vTest.dot(outHit - b) < 0.0f) return false;
		vTest = Normal.crossProduct(a - c);
		if (vTest.dot(outHit - a) < 0.0f) return false;

		return true;
	}

	Vector3 Poly::getSurfaceNormal() const
	{
		auto u = (a - b);
		auto v = (a - c);
		auto res = u.crossProduct(v);
		res.normalise();
		return res;
	}

	Vector3 Poly::getCentrePoint() const
	{
		return (a + b + c) / 3.0f;
	}

	const Vector3& Poly::getClosestPoint(const Vector3& pos) const
	{
		float a_dist = a.distance(pos);
		float b_dist = b.distance(pos);
		float c_dist = c.distance(pos);
		if (a_dist < b_dist
			&& a_dist < c_dist
			)
		{
			return a;
		}
		if (b_dist < a_dist
			&& b_dist < c_dist
			)
		{
			return b;
		}
		return c;
	}

	float Poly::distance(const Vector3& pos) const
	{
		return std::max(a.distance(pos), std::max(b.distance(pos), c.distance(pos)));
	}
}
